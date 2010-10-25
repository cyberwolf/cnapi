<?php

function _cnapi_parse($request, $result) {
  $data = array();

  $xml = new SimpleXMLElement($result);
  
  _cnapi_register_default_namespace($xml, 'c');
  
  if ($request['action'] == 'detail' && $request['type'] == 'event') {
    $xml_event = array_pop($xml->xpath('/c:cdbxml/c:events/c:event'));

    $data = _cnapi_parse_event($xml_event);
    print "<pre>".print_r($data, TRUE)."</pre>";
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

function _cnapi_parse_event($xml, $return_full = TRUE) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_event_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['headings'] = _cnapi_parse_headings($xml);
  $data['calendar'] = _cnapi_parse_calendar($xml);
  $data['contactinfo'] = _cnapi_parse_contactinfo($xml);
  $data['location']['address'] = _cnapi_parse_address(array_pop($xml->xpath('c:location/c:address')));
  $data['location']['actor'] = _cnapi_parse_actor(array_pop($xml->xpath('c:location/c:actor')), FALSE);
  $data['organiser'] = _cnapi_parse_actor(array_pop($xml->xpath('c:organiser/c:actor')), FALSE);
  
  if (!$return_full) {
    return $data['cdbid'];
  }
  
  return $data;
}

function _cnapi_parse_actor($xml, $return_full = TRUE) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  
  // @todo implement this
  
  return $data['cdbid'];
}

function _cnapi_parse_calendar($xml) {
  // @todo implement this
}

function _cnapi_parse_contactinfo($xml) {
  // @todo implement this
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

function _cnapi_parse_address($xml) {
  // @todo implement this
}

function _cnapi_parse_headings($xml) {
  _cnapi_register_default_namespace($xml, 'c');

  return _cnapi_xpath_str($xml, 'c:headings/c:heading/@id', TRUE);
}

function _cnapi_parse_event_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:eventdetails/c:eventdetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_pop($xml->xpath('c:eventdetails/c:eventdetail[@lang="' . $language . '"]'));
    $details[$language] = _cnapi_parse_event_detail($xml_detail);
  }

  return $details;
}

function _cnapi_parse_event_detail($xml) {
  $detail = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $detail['title'] = _cnapi_xpath_str($xml, 'c:title');
  $detail['shortdescription'] = _cnapi_xpath_str($xml, 'c:shortdescription');
  $detail['longdescription'] = _cnapi_xpath_str($xml, 'c:longdescription');
  $detail['calendarsummary'] = _cnapi_xpath_str($xml, 'c:calendarsummary');
  $detail['performers'] = _cnapi_parse_performers($xml);
  $detail['media'] = _cnapi_parse_media($xml);
  $detail['price'] = _cnapi_parse_price($xml);

  return $detail;
}

function _cnapi_parse_performers($xml) {
  $performers = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $performers['labels'] = _cnapi_xpath_str($xml, 'c:performers/c:performer/c:label', TRUE);
  
  $xml_actors = $xml->xpath('c:performers/c:performer/c:actor');
  foreach ($xml_actors as $xml_actor) {
    $performers['actors'][] = _cnapi_parse_actor($xml_actor, FALSE);
  }

  return $performers;
}

function _cnapi_parse_media($xml) {
  // @todo implement this
}

function _cnapi_parse_price($xml) {
  // @todo implement this
}