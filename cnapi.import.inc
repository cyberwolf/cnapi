<?php

/**
 * @file
 * Functions to import cities, headings, ... from the Cultuunret API server.
 */

/**
 * Location of the XML files containg cities, headings, ... on the CultuurNet API server. The path is relative to the cnapi_api_location configuration variable.
 */
define('CNAPI_XML_OUTPUT_TYPE', 'output_type.xml');
define('CNAPI_XML_CITY', 'city.xml');
define('CNAPI_XML_REGION', 'region.xml');
define('CNAPI_XML_CITY_REGION',  'city_region.xml');
define('CNAPI_XML_DIMENSION',  'dimension.xml');
define('CNAPI_XML_CATEGORISATION',  'categorisation.xml');
define('CNAPI_XML_HEADING',  'heading.xml');
define('CNAPI_XML_HEADING_CATEGORISATION',  'heading_categorisation.xml');

/**
 * Import the cities, headings, ... from the CultuurNet API XML files to the local database tables.
 */
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
    'did'       => 0,
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
    'did'       => array('path' => '@dimension', 'parser' => 'intval'),
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

/**
 * Import an XML (like the one containing cities, headings, ...) from the the CultuurNet API server to a local database table.
 *
 * @param $xml
 *   The path of the XML files, relative to the cnapi_api_location configuration variable.
 * @param $path
 *   The XPath path that represents the nodes to be imported as individual rows in the destination table.
 * @param $mapping
 *   An associative array representing the mappings of all fields. The key of each row in the array represents the field name of the local table. The value of each row is an associative array having keys 'path' and 'parser'. The value of 'path' is the XPath path relative to a node from $path. The value of 'parser' is the function (intval, strval, ...) that should be used to transform the SimpleXML element to a value.
 * @param $table
 *   The table to import the elements in.
 * @param $primary_key
 *   The primary key of the table as is should be used in drupal_write_record.
 * @param $truncate
 *   A boolean indicating wether the table should be truncated before importing.
 * @param $multiple_key
 *   If an item representated as node in the XML document should be multiplexed into multiple rows in the local table, $multiple_key indicated what Xpath path should used to indicate the multiple values.
 */
function _cnapi_xml_import($xml, $path, $mappings, $table, $primary_key = array(), $truncate = TRUE, $multiple_key = '') {
  $url = rtrim(variable_get('cnapi_api_location', CNAPI_API_LOCATION), '/') . '/lib/' . variable_get('cnapi_lib_version', '') . '/' . $xml;

  // fetch the xml from the api server
  $xml = cnapi_http_request($url);

  if ($xml && $xml = new SimpleXMLElement($xml)) {
    // truncate the table if necessary
    if ($truncate) {
      db_truncate($table)->execute();
    }

    // iterate over all xml nodes represented by $path
    foreach ($xml->xpath($path) as $row) {
      $object = array();
      foreach ($mappings as $id => $mapping) {
        if (is_array($mapping)) {
          $value = $row->xpath($mapping['path']);
          if (isset($value[0])) {
            $object[$id] = trim(call_user_func($mapping['parser'], $value[0]));
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
          $object[$multiple_key] = trim(call_user_func($mappings[$multiple_key]['parser'], $value));
          $objects[] = $object;
        }
      }

      foreach ($objects as $object) {
        if ($table == 'cnapi_location_hierarchy' && empty($object['pid'])) {
          continue;
        }
        drupal_write_record($table, $object, $primary_key);
      }
    }
  }
  else {
    watchdog('cnapi', 'An error occured while importing values.', array(), WATCHDOG_ERROR);
    return;
  }
}

function cnapi_import_actors() {
  $pagelength = 100;
  $page = 1;
  
  $actors = array();
  
  do {
    $result = cnapi_get_actors(array('pagelength' => $pagelength, 'page' => $page));
    
    $actors = array_merge($actors, $result['data']);
    
    $total = $result['total'];

    $page++;
  }
  while ($pagelength * ($page - 1) < $total);
  
  db_truncate('cnapi_actor')->execute();
  
  foreach ($actors as $actor) {
    $object = array(
      'cdbid' => $actor['cdbid'],
      'name' => $actor['title'],
      'zip' => $actor['zip'],
      'city' => $actor['city'],
    );

    drupal_write_record('cnapi_actor', $object);
  }
}