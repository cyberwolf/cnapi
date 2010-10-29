<?php
// $Id$

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
      $sql = "SELECT cl.lid, cl.zip, cl.name, cl2.lid as pid, cl2.zip as parent_zip, cl2.name as parent_name FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 0";
      $sql .= " UNION ";
      $sql .= "SELECT cl.lid, cl.zip, cl.name, 0 as pid, 0 as parent_zip, '' as parent_name FROM {cnapi_location} cl WHERE cl.did = 0 AND cl.lid NOT IN (SELECT cl.lid FROM {cnapi_location} cl INNER JOIN {cnapi_location_hierarchy} clh ON clh.lid = cl.lid INNER JOIN {cnapi_location} cl2 ON cl2.lid = clh.pid WHERE cl.did = 0 AND cl2.did = 0)";
      
      $sql = "SELECT zip, IF(pid = 0, name, SUBSTRING(name, 1, LENGTH(name) - LENGTH(CONCAT(' ', '(', parent_name, ')')))) as name, lid FROM (" . $sql . ") AS tmp";
      
      $items = db_query($sql, array(':zip' => $zip, ':name' => $name))->fetchAll();
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

function _cnapi_get_tree($type, $pid, &$tree, $flat) {
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
    foreach ($children as $child) {
      // setting item
      $tree[$child] = $flat[$child];
      
      // setting children recursively
      _cnapi_get_tree($type, $child, $tree[$child]['children'], $flat);
      if (empty($tree[$child]['children'])) {
        unset($tree[$child]['children']);
      }
    }
  }
}