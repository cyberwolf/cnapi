<?php
// $Id$

function cnapi_location_parse($input = '') {
  // empty input never parses succesfully
  if (empty($input)) {
    return FALSE;
  }

  // 9000, 9000 Gent, 9000 - Gent, 9000-Gent, ...
  $matches = array();
  $pattern_zip_name = preg_match_all("/^\s*(\d+)\s*-{0,1}\s*(.*)\s*$/", $input, $matches);
  if ($pattern_zip_name) {
    $zip = $matches[1][0];
    $name = $matches[2][0];

    if (!empty($name)) {
      $locations = db_query("SELECT * FROM {cnapi_location} cl WHERE zip = :zip AND (LOWER(name) = :name OR LOWER(name) LIKE :fullname)", array(':zip' => $zip, ':name' => strtolower($name), ':fullname' => strtolower($name) . ' (%)'))->fetchAll(PDO::FETCH_ASSOC);

      if (count($locations) == 1) {
        return array(
          'type' => 'cityid',
          'value' => $locations[0]['lid'],
        );
      }
      else {
        $locations = db_query("SELECT * FROM {cnapi_location} cl WHERE zip = :zip", array(':zip' => $zip))->fetchAll(PDO::FETCH_ASSOC);

        if (count($locations) > 0) {
          return array(
            'type' => 'zip',
            'value' => $zip,
          );
        }
        else {
          return FALSE;
        }
      }
    }
  }

  // ... (Regio)
  $matches = array();
  $pattern_regio = preg_match_all("/\s*(.*)\w*\(regio\)\w*$/i", $input, $matches);
  if ($pattern_regio) {
    $name = $matches[1][0];
  }

  // if no match was found, try the full input for regio matching
  if (empty($name)) {
    $name = $input;
  }

  $locations = db_query("SELECT * FROM {cnapi_location} cl WHERE did = :did AND LOWER(name) = :name", array(':did' => CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY, ':name' => strtolower($name)))->fetchAll(PDO::FETCH_ASSOC);

  if (count($locations) == 1) {
    return array(
      'type' => 'region',
      'value' => $locations[0]['lid'],
    );
  }

  return FALSE;
}

function cnapi_location_autocomplete($input = '') {
  $matches = array();

  if ($input) {
    $sql = "SELECT lid, zip, name, type, sort FROM (";
    $sql .= "SELECT DISTINCT cl2.lid, MIN(cl.zip) AS zip, cl2.name, 'regio' AS type, IF(:input REGEXP '^[0-9]+.*$', min(cl.zip), cl2.name) AS sort FROM {cnapi_location} cl INNER JOIN cnapi_location_hierarchy clh ON clh.lid = cl.lid INNER JOIN cnapi_location cl2 ON cl2.lid = clh.pid WHERE cl2.did = " . CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY . " AND cl.type = 'city' AND (cl2.name LIKE :start OR (cl.name LIKE :start AND cl2.name LIKE :start) OR cl.zip LIKE :start OR (CONCAT(cl.zip, ' ', cl.name) LIKE :start AND cl2.name LIKE :start)) GROUP BY cl2.lid";
    $sql .= " UNION ";
    $sql .= "SELECT cl.lid, cl.zip, cl.name, 'city' AS type, IF(:input REGEXP '^[0-9]+.*$', cl.zip, cl.name) AS sort FROM {cnapi_location} cl WHERE cl.type = 'city' AND (cl.name LIKE :start OR cl.zip LIKE :start OR CONCAT(cl.zip, ' ', cl.name) LIKE :start)";
    $sql .= ") AS tmp ORDER BY sort ASC, name ASC, type DESC";

    $locations = db_query_range($sql, 0, 10, array(':did' => CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY, ':start' => strtolower($input) . '%', ':input' => strtolower($input)))->fetchAll(PDO::FETCH_ASSOC);

    foreach ($locations as $location) {
      if ($location['type'] == 'city') {
        $text = $location['zip'] . ' ' . $location['name'];
      }
      else {
        $text = $location['name'] . ' (Regio)';
      }
      $matches[$text] = $text;
    }
  }

  drupal_json_output($matches);
}

function cnapi_get_provinces() {
  return cnapi_get_regions(CNAPI_LOCATIONS_DIMENSION_PROVINCE);
}

function cnapi_get_regions_administrative() {
  return cnapi_get_regions(CNAPI_LOCATIONS_DIMENSION_ADMINISTRATIVE);
}

function cnapi_get_regions_tourist() {
  return cnapi_get_regions(CNAPI_LOCATIONS_DIMENSION_TOURIST);
}

function cnapi_get_municipalities() {
  return cnapi_get_regions(CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY);
}

function cnapi_get_regions($did) {
  $regions = &drupal_static(__FUNCTION__, array());

  if (!isset($regions[$did])) {
    $regions[$did] = db_query("SELECT cl.lid, cl.name FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.pid = cl.lid WHERE cl.did = :did", array(':did' => $did))->fetchAllAssoc('lid', PDO::FETCH_ASSOC);
  }

  return $regions[$did];
}

function cnapi_get_cities() {
  $cities = &drupal_static(__FUNCTION__, array());

  if (!$cities) {
    $cid = 'cnapi_cities';
    if ($cache = cache_get($cid)) {
      $cities = $cache->data;
    }
    else {
      $sql = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 0";
      $sql .= " UNION ";
      $sql .= "SELECT cl.*, 0 FROM {cnapi_location} cl WHERE cl.did = 0 AND cl.lid NOT IN (SELECT cl.lid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 0)";

      $sql = "SELECT lid, zip, name, pid FROM (" . $sql . ") AS tmp ORDER BY zip ASC";

      $cities = db_query($sql)->fetchAllAssoc('lid', PDO::FETCH_ASSOC);

      cache_set($cid, $cities);
    }
  }

  return $cities;
}

function cnapi_get_cities_tree($tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_cities');
}

function cnapi_get_cityid($zip, $name) {
  $mapping = &drupal_static(__FUNCTION__, array());

  if (!$mapping) {
    $cid = 'cnapi_cityid_mapping';
    if ($cache = cache_get($cid)) {
      $mapping = $cache->data;
    }
    else {
      $sql = "SELECT cl.lid, cl.zip, SUBSTRING(cl.name, 1, LENGTH(cl.name) - LENGTH(CONCAT(' ', '(', cl2.name, ')'))) AS name FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 0";
      $sql .= " UNION ";
      $sql .= "SELECT cl.lid, cl.zip, cl.name FROM {cnapi_location} cl WHERE cl.did = 0";

      $items = db_query($sql)->fetchAll();
      foreach ($items as $item) {
        $mapping[$item->zip][$item->name] = $item->lid;
      }

      cache_set($cid, $mapping);
    }
  }

  return $mapping[$zip][$name];
}

function cnapi_get_locations($did = CNAPI_LOCATIONS_DIMENSION_ADMINISTRATIVE, $include_cities = TRUE) {
  $locations = &drupal_static(__FUNCTION__, array());

  if (!isset($locations[$did][$include_cities])) {
    $cid = sprintf('cnapi_locations:%d:%d', $did, $include_cities ? 1 : 0);

    if ($cache = cache_get($cid)) {
      $locations[$did][$include_cities] = $cache->data;
    }
    else {
      $sql_parts = array();

      $sql_parts[] = "SELECT cl.*, 0 AS pid FROM {cnapi_location} cl WHERE did = 1";
      $sql_parts[] = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid WHERE cl.did = :did";
      $sql_parts[] = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = :did AND cl2.did = :did";

      $params[':did'] = $did;

      if ($include_cities) {
        $sql_parts[] = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = :did";
      }

      $sql = implode(' UNION ', $sql_parts);

      $locations[$did][$include_cities] = db_query($sql, $params)->fetchAllAssoc('lid', PDO::FETCH_ASSOC);

      cache_set($cid, $locations[$did][$include_cities]);
    }
  }

  return $locations[$did][$include_cities];
}

function cnapi_get_locations_tree($did = CNAPI_LOCATIONS_DIMENSION_ADMINISTRATIVE, $include_cities = TRUE, $tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_locations', array($did, $include_cities));
}

function cnapi_get_output_types() {
  $output_types = &drupal_static(__FUNCTION__, NULL);

  if (!$output_types) {
    $output_types = db_query("SELECT tid, name, region_dimension FROM {cnapi_output_type}")->fetchAllAssoc('tid', PDO::FETCH_ASSOC);
  }

  return $output_types;
}

function cnapi_get_dimensions($key = 'machinename') {
  $dimensions = &drupal_static(__FUNCTION__, array());

  if (!isset($dimensions[$key])) {
    $dimensions[$key] = db_query("SELECT did, machinename, name FROM {cnapi_dimension}")->fetchAllAssoc($key, PDO::FETCH_ASSOC);
  }

  return $dimensions[$key];
}

function cnapi_get_event_types() {
  return _cnapi_get_categories(CNAPI_DIMENSION_EVENTTYPE);
}

function cnapi_get_event_types_tree($tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_event_types', array(), '');
}

function cnapi_get_themes() {
  return _cnapi_get_categories(CNAPI_DIMENSION_THEME);
}

function cnapi_get_themes_tree($tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_themes', array(), '');
}

function cnapi_get_targetaudiences() {
  $targetaudiences = &drupal_static(__FUNCTION__, array());

  if (empty($targetaudiences)) {
    $targetaudiences = db_query("SELECT cid, pid, name, did FROM {cnapi_category} WHERE did = :did AND NOT pid = '2.0.0.0.0'", array(':did' => CNAPI_DIMENSION_TARGETAUDIENCE))->fetchAllAssoc('cid', PDO::FETCH_ASSOC);
  }

  return $targetaudiences;
}

function cnapi_get_targetaudiences_tree($tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_targetaudiences', array(), '');
}

function cnapi_get_facilities() {
  return _cnapi_get_categories(CNAPI_DIMENSION_FACILITY);
}

function cnapi_get_publicscopes() {
  return _cnapi_get_categories(CNAPI_DIMENSION_PUBLICSCOPE);
}

function cnapi_get_actortypes() {
  return _cnapi_get_categories(CNAPI_DIMENSION_ACTORTYPE);
}

function cnapi_get_actortypes_tree($tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_actortypes', array(), '');
}

function cnapi_get_municipals() {
  return _cnapi_get_categories(CNAPI_DIMENSION_MUNICIPAL);
}

function cnapi_get_ipe() {
  return _cnapi_get_categories(CNAPI_DIMENSION_IPE);
}

function cnapi_get_misc() {
  return _cnapi_get_categories(CNAPI_DIMENSION_MISC);
}

function cnapi_get_age_types() {
  $types = &drupal_static(__FUNCTION__, array());

  if (empty($types)) {
    $types = db_query("SELECT cid, pid, name FROM {cnapi_category} WHERE did = :did AND pid = '2.0.0.0.0'", array(':did' => CNAPI_DIMENSION_TARGETAUDIENCE))->fetchAllAssoc('cid', PDO::FETCH_ASSOC);
  }

  return $types;
}

function _cnapi_get_categories($did) {
  $categories = &drupal_static(__FUNCTION__, array());

  if (!isset($categories[$did])) {
    $hierarchical = array(
      CNAPI_DIMENSION_EVENTTYPE,
      CNAPI_DIMENSION_THEME,
      CNAPI_DIMENSION_TARGETAUDIENCE,
      CNAPI_DIMENSION_ACTORTYPE,
    );
    
    $categories[$did] = db_query("SELECT cid" . (in_array($did, $hierarchical) ? ", pid" : "") . ", name FROM {cnapi_category} WHERE did = :did", array(':did' => $did))->fetchAllAssoc('cid', PDO::FETCH_ASSOC);
  }

  return $categories[$did];
}

function cnapi_get_headings($tid) {
  $headings = &drupal_static(__FUNCTION__, NULL);

  if (!$headings) {
    $headings = db_query("SELECT hid, pid, name, weight FROM {cnapi_heading} WHERE tid = :tid ORDER BY pid, weight", array(':tid' => $tid))->fetchAllAssoc('hid', PDO::FETCH_ASSOC);
  }

  return $headings;
}

function cnapi_get_headings_tree($tid, $tree_type = 'tree') {
  return _cnapi_get_tree(__FUNCTION__, $tree_type, 'cnapi_get_headings', array($tid));
}

function cnapi_get_datetype($id) {
  $types = cnapi_get_datetypes();

  if (array_key_exists($id, $types)) {
    return $types[$id];
  }

  return $id;
}

function cnapi_get_datetypes() {
  $map = &drupal_static(__FUNCTION__, array());

  if (empty($map)) {
    $map = array(
      'today'        => t('Today'),
      'tomorrow'     => t('Tomorrow'),
      'thisweek'     => t('This week'),
      'thisweekend'  => t('This weekend'),
      'nextweekend'  => t('Next weekend'),
      'thismonth'    => t('This month'),
      'next30days'   => t('Next @days days', array('@days' => 30)),
      'next3months'  => t('Next @months months', array('@months' => 3)),
      'next6months'  => t('Next @months months', array('@months' => 6)),
      'next12months' => t('Next @months months', array('@months' => 12)),
    );
  }

  return $map;
}