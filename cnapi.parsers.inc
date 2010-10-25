<?php
// $Id$

function _cnapi_parse($request, $result) {
  $data = array();

  $xml = new SimpleXMLElement($result);
  
  _cnapi_register_default_namespace($xml, 'c');
  
  // detail
  if ($request['action'] == 'detail') {
    
    // event
    if ($request['type'] == 'event') {
      $xml_event = array_shift($xml->xpath('/c:cdbxml/c:events/c:event'));
    
      $data = _cnapi_parse_event($xml_event);
    }
    
    // production
    elseif ($request['type'] == 'production') {
      $xml_production = array_shift($xml->xpath('/c:cdbxml/c:productions/c:production'));
    
      $data = _cnapi_parse_production($xml_production);
    }
    
    // actor
    elseif ($request['type'] == 'actor') {
      $xml_actor = array_shift($xml->xpath('/c:cdbxml/c:actors/c:actor'));
    
      $data = _cnapi_parse_actor($xml_actor);
    }
  }
  
  // list_detail
  elseif ($request['action'] == 'list_detail') {
    $data['total'] = _cnapi_xpath_str($xml, '/c:c/c:nofrecords');
    $data['data'] = array();
    
    // event
    if ($request['type'] == 'event') {
      $xml_events = $xml->xpath('/c:cdbxml/c:events/c:event');
    
      foreach ($xml_events as $xml_event) {
        $data['data'][] = _cnapi_parse_event($xml_event);
      }
    }
    
    // event
    elseif ($request['type'] == 'production') {
      $xml_productions = $xml->xpath('/c:cdbxml/c:productions/c:production');
    
      foreach ($xml_productions as $xml_production) {
        $data['data'][] = _cnapi_parse_production($xml_production);
      }
    }
    
    // actor
    elseif ($request['type'] == 'actor') {
      $xml_actors = $xml->xpath('/c:cdbxml/c:actors/c:actor');
    
      foreach ($xml_actors as $xml_actor) {
        $data['data'][] = _cnapi_parse_actor($xml_actor);
      }
    }
  }
  
  // list_summary
  elseif ($request['action'] == 'list_summary') {
    $data['total'] = _cnapi_xpath_str($xml, '/c:c/c:nofrecords');
    
    $items_path = '/c:cdbxml/c:list/c:item';
    if ($request['type'] == 'production') {
      $items_path = '/c:cdbxml/c:production/c:item';
    }
    
    foreach ($xml->xpath($items_path) as $item) {
      $object = _cnapi_parse_list_item($request['type'], $item);

      $data['data'][] = $object;
    }
  }
  
  return $data;
}

function _cnapi_parse_list_item($type, $xml) {
  $object['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $object['title'] = _cnapi_xpath_str($xml, '@title');
  $object['shortdescription'] = _cnapi_xpath_str($xml, '@shortdescription');
  $object['thumbnail'] = _cnapi_xpath_str($xml, '@thumbnail');
  $object['created'] = strtotime(_cnapi_xpath_str($xml, '@created'));
  
  if (in_array($type, array('event', 'actor'))) {
    $object['city'] = _cnapi_xpath_str($xml, '@city');
    $object['zip'] = _cnapi_xpath_str($xml, '@zip');
    $object['address'] = _cnapi_xpath_str($xml, '@address');
    $latlng = explode(';', _cnapi_xpath_str($xml, '@latlng'));
    $object['gis']['lat'] = $latlng[0];
    $object['gis']['lng'] = $latlng[1];
  }
  
  if ($type == 'event') {
    $object['calendarsummary'] = _cnapi_xpath_str($xml, '@calendarsummary');
    $object['headings'] = explode(';', _cnapi_xpath_str($xml, '@headingid'));
    $object['locationid'] = _cnapi_xpath_str($xml, '@locationid');
    $object['available_to'] = strtotime(_cnapi_xpath_str($xml, '@available_to'));
  }
  
  if (in_array($type, array('event', 'production'))) {
    $object['agefrom'] = _cnapi_xpath_int($xml, '@agefrom');
    $object['performers'] = explode(';', _cnapi_xpath_str($xml, '@performers'));
  }
  
  return $object;
}

function _cnapi_parse_event($xml) {
  $data = array();

  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_event_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['headings'] = _cnapi_parse_headings($xml);
  $data['agefrom'] = _cnapi_xpath_int($xml, '@agefrom');
  $data['calendar'] = _cnapi_parse_calendar($xml);
  $data['contactinfo'] = _cnapi_parse_contactinfo(array_shift($xml->xpath('c:contactinfo')));
  $data['location']['address'] = _cnapi_parse_address(array_shift($xml->xpath('c:location/c:address')));
  $data['location']['actor'] = _cnapi_parse_actor(array_shift($xml->xpath('c:location/c:actor')));
  $data['organiser'] = _cnapi_parse_actor(array_shift($xml->xpath('c:organiser/c:actor')));
  
  // caching this object
  $request = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $data['cdbid']));
  $cid = cnapi_cache_cid($request);
  _cnapi_cache_set($request, $data);
  
  return $data;
}

function _cnapi_parse_production($xml) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_production_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['agefrom'] = _cnapi_xpath_int($xml, '@agefrom');
  if ($xml_related_events = $xml->xpath('c:relatedevents')) {
    $data['related_events'] = _cnapi_parse_related_events(array_shift($xml_related_events));
  }  
  
  // caching this object
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $data['cdbid']));
  
  if ($xml->xpath('c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'list';
  }
  elseif ($xml->xpath('c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'detail';
  }
  
  $cid = cnapi_cache_cid($request);
  _cnapi_cache_set($request, $data);
  
  return $data;
}

function _cnapi_parse_actor($xml) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_actor_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['contactinfo'] = _cnapi_parse_contactinfo(array_shift($xml->xpath('c:contactinfo')));
  
  // caching this object
  $request = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $data['cdbid']));
  $cid = cnapi_cache_cid($request);
  _cnapi_cache_set($request, $data);
  
  return $data;
}

function _cnapi_parse_event_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:eventdetails/c:eventdetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_shift($xml->xpath('c:eventdetails/c:eventdetail[@lang="' . $language . '"]'));
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

function _cnapi_parse_production_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:productiondetails/c:productiondetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_shift($xml->xpath('c:productiondetails/c:productiondetail[@lang="' . $language . '"]'));
    $details[$language] = _cnapi_parse_production_detail($xml_detail);
  }

  return $details;
}

function _cnapi_parse_production_detail($xml) {
  $detail = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $detail['title'] = _cnapi_xpath_str($xml, 'c:title');
  $detail['shortdescription'] = _cnapi_xpath_str($xml, 'c:shortdescription');
  $detail['longdescription'] = _cnapi_xpath_str($xml, 'c:longdescription');
  $detail['performers'] = _cnapi_parse_performers($xml);
  $detail['media'] = _cnapi_parse_media($xml);

  return $detail;
}

function _cnapi_parse_actor_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:actordetails/c:actordetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_shift($xml->xpath('c:actordetails/c:actordetail[@lang="' . $language . '"]'));
    $details[$language] = _cnapi_parse_actor_detail($xml_detail);
  }

  return $details;
}

function _cnapi_parse_actor_detail($xml) {
  $detail = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $detail['title'] = _cnapi_xpath_str($xml, 'c:title');
  $detail['shortdescription'] = _cnapi_xpath_str($xml, 'c:shortdescription');
  $detail['longdescription'] = _cnapi_xpath_str($xml, 'c:longdescription');
  $detail['media'] = _cnapi_parse_media($xml);

  return $detail;
}

function _cnapi_parse_related_events($xml) {
  $related_events = array();

  _cnapi_register_default_namespace($xml, 'c');
  
  // related events as list_summary
  if ($items = $xml->xpath('c:list/c:item')) {
    foreach ($items as $item) {
      $object = _cnapi_parse_list_item('event', $item);

      $related_events[] = $object;
    }
  }

  // related events as list_detail
  elseif ($items = $xml->xpath('c:event')) {
    foreach ($items as $item) {
      $object = _cnapi_parse_event($item);

      $related_events[] = $object;
    }
  }

  // related events as list of cdbid
  else {
    $related_events = _cnapi_xpath_str($xml, 'c:id/@cdbid', TRUE);
  }
  
  return $related_events;
}

function _cnapi_parse_calendar($xml) {
  _cnapi_register_default_namespace($xml, 'c');
  
  // @todo implement this
}

function _cnapi_parse_contactinfo($xml) {
  _cnapi_register_default_namespace($xml, 'c');

  $contactinfo['address'] = _cnapi_parse_address(array_shift($xml->xpath('c:address')));

  $contactinfo['main']['mail'] = _cnapi_xpath_str($xml, 'c:mail[@main="true"]');
  $contactinfo['main']['phone'] = _cnapi_xpath_str($xml, 'c:phone[@main="true" and @type="phone"]');
  $contactinfo['main']['fax'] = _cnapi_xpath_str($xml, 'c:phone[@main="true" and @type="fax"]');
  $contactinfo['main']['url'] = _cnapi_xpath_str($xml, 'c:url[@main="true"]');

  $contactinfo['reservation']['mail'] = _cnapi_xpath_str($xml, 'c:mail[@reservation="true"]');
  $contactinfo['reservation']['phone'] = _cnapi_xpath_str($xml, 'c:phone[@reservation="true" and @type="phone"]');
  $contactinfo['reservation']['fax'] = _cnapi_xpath_str($xml, 'c:phone[@reservation="true" and @type="fax"]');
  $contactinfo['reservation']['url'] = _cnapi_xpath_str($xml, 'c:url[@reservation="true"]');

  $contactinfo['other']['mail'] = _cnapi_xpath_str($xml, 'c:mail[not(@main="true" or @reservation="true")]', TRUE);
  $contactinfo['other']['phone'] = _cnapi_xpath_str($xml, 'c:phone[not(@main="true" or @reservation="true") and @type="phone"]', TRUE);
  $contactinfo['other']['fax'] = _cnapi_xpath_str($xml, 'c:phone[not(@main="true" or @reservation="true") and @type="fax"]', TRUE);
  $contactinfo['other']['url'] = _cnapi_xpath_str($xml, 'c:url[not(@main="true" or @reservation="true")]', TRUE);

  return $contactinfo;
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
  _cnapi_register_default_namespace($xml, 'c');
  
  $address['street'] = _cnapi_xpath_str($xml, 'c:physical/c:street');
  $address['housenr'] = _cnapi_xpath_str($xml, 'c:physical/c:housenr');
  $address['zipcode'] = _cnapi_xpath_str($xml, 'c:physical/c:zipcode');
  $address['city'] = _cnapi_xpath_str($xml, 'c:physical/c:city');
  $address['country'] = _cnapi_xpath_str($xml, 'c:physical/c:country');
  $address['gis']['lat'] = _cnapi_xpath_str($xml, 'c:physical/c:gis/xcoordinate');
  $address['gis']['lng'] = _cnapi_xpath_str($xml, 'c:physical/c:gis/ycoordinate');
  
  // @todo add cityid
  
  return $address;
}

function _cnapi_parse_headings($xml) {
  _cnapi_register_default_namespace($xml, 'c');

  return _cnapi_xpath_str($xml, 'c:headings/c:heading/@id', TRUE);
}

function _cnapi_parse_performers($xml) {
  $performers = array();
  
  _cnapi_register_default_namespace($xml, 'c');

  $performers['labels'] = _cnapi_xpath_str($xml, 'c:performers/c:performer/c:label', TRUE);
  
  if ($xml_actors = $xml->xpath('c:performers/c:performer/c:actor')) {
    foreach ($xml_actors as $xml_actor) {
      $performers['actors'][] = _cnapi_parse_actor($xml_actor);
    }
  }

  return $performers;
}

function _cnapi_parse_media($xml) {
  _cnapi_register_default_namespace($xml, 'c');
  
  // @todo implement this
}

function _cnapi_parse_price($xml) {
  _cnapi_register_default_namespace($xml, 'c');
  
  $price['value'] = _cnapi_xpath_float($xml, 'c:pricevalue');
  $price['description'] = _cnapi_xpath_str($xml, 'c:pricedescription');
  
  return $price;
}