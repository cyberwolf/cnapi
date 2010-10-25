<?php

function _cnapi_parse($request, $result) {
  $data = array();

  $xml = new SimpleXMLElement($result);
  
  _cnapi_register_default_namespace($xml, 'c');
  
  if ($request['action'] == 'detail' && $request['type'] == 'event') {
    $detail = $xml->xpath('/c:cdbxml/c:events/c:event');
    $categories = _cnapi_parse_categories($detail[0]);
  }
  
  if (in_array($request['action'], array('list_detail', 'list_summary'))) {
    $data['total'] = _cnapi_xpath_str($xml, '/c:c/c:nofrecords');
  }
  
  if ($request['action'] == 'summary') {
    $items_path = '/c:cdbxml/c:list/c:item';
    if ($request['type'] == 'production') {
      $items_path = '/c:cdbxml/c:production/c:item';
    }
    
    foreach ($xml->xpath($items_path) as $item) {
      $object = array();
      
      $object['cdbid'] = _cnapi_xpath_str($item, '@cdbid');
      $object['title'] = _cnapi_xpath_str($item, '@title');
      $object['shortdescription'] = _cnapi_xpath_str($item, '@shortdescription');
      $object['thumbnail'] = _cnapi_xpath_str($item, '@thumbnail');
      $object['created'] = strtotime(_cnapi_xpath_str($item, '@created'));
      
      if (in_array($request['type'], array('event', 'actor'))) {
        $object['city'] = _cnapi_xpath_str($item, '@city');
        $object['zip'] = _cnapi_xpath_str($item, '@zip');
        $object['address'] = _cnapi_xpath_str($item, '@address');
        $latlng = explode(';', _cnapi_xpath_str($item, '@latlng'));
        $object['lat'] = $latlng[0];
        $object['lng'] = $latlng[1];
      }
      
      if ($request['type'] == 'event') {
        $object['calendarsummary'] = _cnapi_xpath_str($item, '@calendarsummary');
        $object['headingid'] = _cnapi_xpath_int($item, '@headingid', TRUE);
        $object['locationid'] = _cnapi_xpath_str($item, '@locationid');
        $object['agefrom'] = _cnapi_xpath_int($item, '@agefrom');
        $object['performers'] = _cnapi_xpath_str($item, '@performers', TRUE);
        $object['available_to'] = strtotime(_cnapi_xpath_str($item, '@available_to'));
      }
      
      if ($request['type'] == 'production') {
        $object['agefrom'] = _cnapi_xpath_int($item, '@agefrom');
        $object['performers'] = _cnapi_xpath_str($item, '@performers', TRUE);
      }

      $data['data'][] = $object;
    }
  }
  
  return $data;
}

function _cnapi_parse_categories($xml) {
  $categories = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $types = _cnapi_xpath_str($xml, 'c:categories/c:category/@type', TRUE);
  foreach ($types as $type) {
    $categories[$type] = _cnapi_xpath_str($xml, 'c:categories/c:category[@type="' . $type . '"]/@catid', TRUE);
  }

  return $categories;
}