<?php
// $Id$

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
    $regions[$did] = db_query("SELECT cl.* FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.pid = cl.lid WHERE cl.did = :did", array(':did' => $did))->fetchAllAssoc('lid', PDO::FETCH_ASSOC);
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
      
      $sql = "SELECT * FROM (" . $sql . ") AS tmp ORDER BY zip ASC";
      
      $cities = db_query($sql)->fetchAllAssoc('lid', PDO::FETCH_ASSOC);
      
      cache_set($cid, $cities);
    }
  }
  
  return $cities;
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

function cnapi_get_cities_tree() {
  $cities = cnapi_get_cities();

  $tree = array();
  _cnapi_get_tree('cities', 0, $tree, $cities);
  
  return $tree;
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
      $sql_parts[] = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 4 AND cl2.did = :did";
        
      $params[':did'] = $did;
      
      if ($include_cities) {
        $sql_parts[] = "SELECT cl.*, clh.pid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 4";
      }
      
      $sql = implode(' UNION ', $sql_parts);
      
      $locations[$did][$include_cities] = db_query($sql, $params)->fetchAllAssoc('lid', PDO::FETCH_ASSOC);
      
      cache_set($cid, $locations[$did][$include_cities]);
    }
  }

  return $locations[$did][$include_cities];
}

function cnapi_get_locations_tree($did = CNAPI_LOCATIONS_DIMENSION_ADMINISTRATIVE, $include_cities = TRUE) {
  $locations = cnapi_get_locations($did, $include_cities);

  $tree = array();
  _cnapi_get_tree('locations', 0, $tree, $locations);
  
  return $tree;
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
  return cnapi_get_categories(CNAPI_DIMENSION_EVENTTYPE);
}

function cnapi_get_themes() {
  return cnapi_get_categories(CNAPI_DIMENSION_THEME);
}

function cnapi_get_targetaudiences() {
  return cnapi_get_categories(CNAPI_DIMENSION_TARGETAUDIENCE);
}

function cnapi_get_facilities() {
  return cnapi_get_categories(CNAPI_DIMENSION_FACILITY);
}

function cnapi_get_publicscopes() {
  return cnapi_get_categories(CNAPI_DIMENSION_PUBLICSCOPE);
}

function cnapi_get_actortypes() {
  return cnapi_get_categories(CNAPI_DIMENSION_ACTORTYPE);
}

function cnapi_get_municipals() {
  return cnapi_get_categories(CNAPI_DIMENSION_MUNICIPAL);
}

function cnapi_get_ipe() {
  return cnapi_get_categories(CNAPI_DIMENSION_IPE);
}

function cnapi_get_misc() {
  return cnapi_get_categories(CNAPI_DIMENSION_MISC);
}

function cnapi_get_age_types() {
  $types = &drupal_static(__FUNCTION__, array());
  
  if (!isset($categories[$did])) {
    $types = db_query("SELECT cid, pid, name, did FROM {cnapi_category} WHERE did = :did AND cid LIKE '2.2.%'", array(':did' => CNAPI_DIMENSION_TARGETAUDIENCE))->fetchAllAssoc('cid', PDO::FETCH_ASSOC);
  }
  
  return $types;
}

function cnapi_get_categories($did) {
  $categories = &drupal_static(__FUNCTION__, array());
  
  if (!isset($categories[$did])) {
    $categories[$did] = db_query("SELECT cid, pid, name, did FROM {cnapi_category} WHERE did = :did", array(':did' => $did))->fetchAllAssoc('cid', PDO::FETCH_ASSOC);
  }
  
  return $categories[$did];
}

function cnapi_get_categories_tree($did) {
  $categories = cnapi_get_categories($did);
  
  $tree = array();
  _cnapi_get_tree('categories', '', $tree, $categories);
  
  return $tree;
}

function cnapi_get_headings($tid) {
  $headings = &drupal_static(__FUNCTION__, NULL);
  
  if (!$headings) {
    $headings = db_query("SELECT hid, pid, tid, name, weight FROM {cnapi_heading} WHERE tid = :tid ORDER BY pid, weight", array(':tid' => $tid))->fetchAllAssoc('hid', PDO::FETCH_ASSOC);
  }
  
  return $headings;
}

function cnapi_get_headings_tree($tid) {
  $headings = cnapi_get_headings($tid);
  
  $tree = array();
  _cnapi_get_tree('headings', 0, $tree, $headings);
  
  return $tree;
}

function _cnapi_get_tree($type, $pid, &$tree, $flat, $tree_type = 'tree', $depth = 0) {
  $parents = &drupal_static(__FUNCTION__, NULL);
  
  // setting parents if we haven't yet 
  if (!isset($parents[$type])) {
    foreach ($flat as $id => $item) {
      $parents[$type][$item['pid']][] = $id;
    }
  }
  
  // getting children ids for current item
  $children = isset($parents[$type][$pid]) ? $parents[$type][$pid] : array();

  if ($children) {
    $child_depth = $depth + 1;

    foreach ($children as $child) {
      // setting item
      $tree[$child] = $flat[$child];
      
      // setting item depth on flat tree
      if ($tree_type == 'flat') {
        $tree[$child]['depth'] = $depth;
      }
      
      // setting children recursively
      if ($tree_type == 'tree') {
        _cnapi_get_tree($type, $child, $tree[$child]['children'], $flat, $tree_type, $child_depth);
        if (empty($tree[$child]['children'])) {
          unset($tree[$child]['children']);
        }
      }
      else {
        _cnapi_get_tree($type, $child, $tree, $flat, $tree_type, $child_depth);
      }
    }
  }
}

function _cnapi_get_select_options($tree, $id_key = 'id', $name_key = 'name', $blank = NULL, $dashes = TRUE) {
  $options = array();

  if ($blank) {
    $options[-1] = $blank;
  }
  
  if ($tree) {
    foreach ($tree as $item) {
      $options[$item[$id_key]] = ($dashes ? str_repeat('-', $item['depth']) : '') . $item[$name_key];
    }
  }

  return $options;
}