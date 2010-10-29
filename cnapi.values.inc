<?php
// $Id$

define('CNAPI_XML_OUTPUT_TYPE', '/lib/output_type.xml');
define('CNAPI_XML_CITY', '/lib/city.xml');
define('CNAPI_XML_REGION', '/lib/region.xml');
define('CNAPI_XML_CITY_REGION',  '/lib/city_region.xml');
define('CNAPI_XML_DIMENSION',  '/lib/dimension.xml');
define('CNAPI_XML_CATEGORISATION',  '/lib/categorisation.xml');
define('CNAPI_XML_HEADING',  '/lib/heading.xml');
define('CNAPI_XML_HEADING_CATEGORISATION',  '/lib/heading_categorisation.xml');

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

function cnapi_import() {
  // output types
  $mappings = array(
    'tid'              => array('path' => '@id', 'parser' => 'intval'),
    'name'             => array('path' => '@title', 'parser' => 'strval'),
    'region_dimension' => array('path' => '@dimension_region', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_OUTPUT_TYPE, '/Output_type/row', $mappings, 'cnapi_output_type');

  // dimensions
  $mappings = array(
    'did'         => array('path' => '@id', 'parser' => 'intval'),
    'machinename' => array('path' => '@value', 'parser' => 'strval'),
    'name'        => array('path' => '@label', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_DIMENSION, '/Dimension/dimension', $mappings, 'cnapi_dimension');

  // categories
  $mappings = array(
    'cid'       => array('path' => '@cnet_id', 'parser' => 'strval'),
    'pid'       => array('path' => '@parent_id', 'parser' => 'strval'),
    'name'      => array('path' => '@title', 'parser' => 'strval'),
    'did'       => array('path' => '@dimension', 'parser' => 'intval'),
  );
  _cnapi_xml_import(CNAPI_XML_CATEGORISATION, '/Categorisation/categorisation', $mappings, 'cnapi_category');

  // headings
  $mappings = array(
    'hid'    => array('path' => '@id', 'parser' => 'intval'),
    'pid'    => array('path' => '@parent_id', 'parser' => 'strval'),
    'weight' => array('path' => '@sort', 'parser' => 'intval'),
    'name'   => array('path' => '@title', 'parser' => 'strval'),
    'tid'    => array('path' => '@output_type_id', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_HEADING, '/Heading/heading', $mappings, 'cnapi_heading');

  // cities
  $mappings = array(
    'lid'       => array('path' => '@id', 'parser' => 'strval'),
    'type'      => 'city',
    'name'      => array('path' => '@city', 'parser' => 'strval'),
    'zip'       => array('path' => '@zip', 'parser' => 'strval'),
    'dimension' => NULL,
  );
  _cnapi_xml_import(CNAPI_XML_CITY, '/City/city', $mappings, 'cnapi_location');

  // cities hierarchy
  $mappings = array(
    'lid'       => array('path' => '@id', 'parser' => 'strval'),
    'pid'       => array('path' => '@parent', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_CITY, '/City/city', $mappings, 'cnapi_location_hierarchy');

  // regions
  $mappings = array(
    'lid'       => array('path' => '@id', 'parser' => 'strval'),
    'type'      => 'region',
    'name'      => array('path' => '@title', 'parser' => 'strval'),
    'zip'       => NULL,
    'dimension' => array('path' => '@dimension', 'parser' => 'intval'),
  );
  _cnapi_xml_import(CNAPI_XML_REGION, '/Region/region', $mappings, 'cnapi_location', array(), FALSE);

  // regions hierarchy
  $mappings = array(
    'lid'       => array('path' => '@id', 'parser' => 'strval'),
    'pid'       => array('path' => 'parents/@parent', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_REGION, '/Region/region', $mappings, 'cnapi_location_hierarchy', array(), FALSE, 'pid');

  // city - regions hierarchy
  $mappings = array(
    'lid' => array('path' => '@id', 'parser' => 'strval'),
    'pid' => array('path' => '@region_id', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_CITY_REGION, '/City_region/city_region', $mappings, 'cnapi_location_hierarchy', array(), FALSE);

  // heading categorisation
  $mappings = array(
    'hid' => array('path' => '@heading_id', 'parser' => 'intval'),
    'cid' => array('path' => '@cnet_id', 'parser' => 'strval'),
  );
  _cnapi_xml_import(CNAPI_XML_HEADING_CATEGORISATION, '/Heading_categorisation/heading_categorisation', $mappings, 'cnapi_heading_category');

  cache_clear_all();

  drupal_set_message(t('Succesfully imported all values.'), 'status');
}

function _cnapi_xml_import($xml, $path, $mappings, $table, $primary_key = array(), $truncate = TRUE, $multiple_key = '') {
  $url = variable_get('cnapi_api_location', CNAPI_API_LOCATION) . $xml;
  $xml = cnapi_http_request($url);

  if ($xml && $xml = new SimpleXMLElement($xml)) {
    if ($truncate) {
      db_truncate($table)->execute();
    }

    foreach ($xml->xpath($path) as $row) {
      $object = array();
      foreach ($mappings as $id => $mapping) {
        if (is_array($mapping)) {
          $value = $row->xpath($mapping['path']);
          if (isset($value[0])) {
            $object[$id] = call_user_func($mapping['parser'], $value[0]);
          }
        }
        else {
          $object[$id] = $mapping;
        }
      }

      $objects = array();

      if (empty($multiple_key)) {
        $objects[] = $object;
      }
      else {
        $values = $row->xpath($mappings[$multiple_key]['path']);
        foreach ($values as $value) {
          $object[$multiple_key] = call_user_func($mappings[$multiple_key]['parser'], $value);
          $objects[] = $object;
        }
      }

      foreach ($objects as $object) {
        drupal_write_record($table, $object, $primary_key);
      }
    }
  }
  else {
    watchdog('cnapi', 'An error occured while importing values.', array(), WATCHDOG_ERROR);
    return;
  }
}