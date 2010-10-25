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
      $xml_event = array_pop($xml->xpath('/c:cdbxml/c:events/c:event'));
    
      $data = _cnapi_parse_event($xml_event);
    }
    
    // production
    elseif ($request['type'] == 'production') {
      $xml_production = array_pop($xml->xpath('/c:cdbxml/c:productions/c:production'));
    
      $data = _cnapi_parse_production($xml_production);
    }
    
    // actor
    elseif ($request['type'] == 'actor') {
      $xml_actor = array_pop($xml->xpath('/c:cdbxml/c:actors/c:actor'));
    
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
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['headings'] = _cnapi_parse_headings($xml);
  $data['agefrom'] = _cnapi_xpath_int($xml, '@agefrom');
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

function _cnapi_parse_production($xml, $return_full = TRUE) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_production_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['agefrom'] = _cnapi_xpath_int($xml, '@agefrom');
  
  if (!$return_full) {
    return $data['cdbid'];
  }
  
  return $data;
}

function _cnapi_parse_actor($xml, $return_full = TRUE) {
  $data = array();
  
  _cnapi_register_default_namespace($xml, 'c');
  
  $data['cdbid'] = _cnapi_xpath_str($xml, '@cdbid');
  $data['detail'] = _cnapi_parse_actor_details($xml);
  $data['categories'] = _cnapi_parse_categories($xml);
  $data['keywords'] = explode(';', _cnapi_xpath_str($xml, 'keywords'));
  $data['contactinfo'] = _cnapi_parse_contactinfo($xml);
  
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

function _cnapi_parse_production_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:productiondetails/c:productiondetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_pop($xml->xpath('c:productiondetails/c:productiondetail[@lang="' . $language . '"]'));
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
  $detail['price'] = _cnapi_parse_price($xml);

  return $detail;
}

function _cnapi_parse_actor_details($xml) {
  $details = array();

  _cnapi_register_default_namespace($xml, 'c');

  $languages = _cnapi_xpath_str($xml, 'c:actordetails/c:actordetail/@lang', TRUE);

  foreach ($languages as $language) {
    $xml_detail = array_pop($xml->xpath('c:actordetails/c:actordetail[@lang="' . $language . '"]'));
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