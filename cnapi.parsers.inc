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
      $xml_event = array_shift(_cnapi_xpath($xml, '/c:cdbxml/c:events/c:event'));
    
      _cnapi_parse_event($data, NULL, $xml_event);
    }
    
    // production
    elseif ($request['type'] == 'production') {
      $xml_production = array_shift(_cnapi_xpath($xml, '/c:cdbxml/c:productions/c:production'));
    
      _cnapi_parse_production($data, NULL, $xml_production);
    }
    
    // actor
    elseif ($request['type'] == 'actor') {
      $xml_actor = array_shift(_cnapi_xpath($xml, '/c:cdbxml/c:actors/c:actor'));
    
      _cnapi_parse_actor($data, NULL, $xml_actor);
    }
  }
  
  // list_detail
  elseif ($request['action'] == 'list_detail') {
    $data['total'] = _cnapi_xpath_str($xml, 'c:nofrecords');
    $data['data'] = array();
    
    // event
    if ($request['type'] == 'event') {
      $xml_events = _cnapi_xpath($xml, '/c:cdbxml/c:events/c:event');
    
      foreach ($xml_events as $xml_event) {
        $event = array();
        _cnapi_parse_event($event, NULL, $xml_event);
        $data['data'][] = $event;
      }
    }
    
    // event
    elseif ($request['type'] == 'production') {
      $xml_productions = _cnapi_xpath($xml, '/c:cdbxml/c:productions/c:production');
    
      foreach ($xml_productions as $xml_production) {
        $production = array();
        _cnapi_parse_production($production, NULL, $xml_production);
        $data['data'][] = $production;
      }
    }
    
    // actor
    elseif ($request['type'] == 'actor') {
      $xml_actors = _cnapi_xpath($xml, '/c:cdbxml/c:actors/c:actor');
    
      foreach ($xml_actors as $xml_actor) {
        $actor = array();
        _cnapi_parse_actor($actor, NULL, $xml_actor);
        $data['data'][] = $actor;
      }
    }
  }
  
  // list_summary
  elseif ($request['action'] == 'list_summary') {
    $data['total'] = _cnapi_xpath_str($xml, 'c:nofrecords');
    
    $items_path = '/c:cdbxml/c:list/c:item';
    if ($request['type'] == 'production') {
      $items_path = '/c:cdbxml/c:production/c:item';
    }
    
    foreach (_cnapi_xpath($xml, $items_path) as $item) {
      $object = array();
      call_user_func('_cnapi_parse_list_item' . $request['type'], $object, $item);

      $data['data'][] = $object;
    }
  }
  
  return $data;
}

function _cnapi_parse_event_list_item(&$data, $$xml) {
  // $data['cdbid']
  _cnapi_parse_str($data, 'cdbid', $xml, '@cdbid');
  
  // $data['title']
  _cnapi_parse_str($data, 'title', $xml, '@title');
  
  // $data['shortdescription']
  _cnapi_parse_str($data, 'shortdescription', $xml, '@shortdescription');
  
  // $data['thumbnail']
  _cnapi_parse_str($data, 'thumbnail', $xml, '@thumbnail');
  
  // $data['city']
  _cnapi_parse_str($data, 'city', $xml, '@city');
  
  // $data['zip']
  _cnapi_parse_str($data, 'zip', $xml, '@zip');
  
  // $data['address']
  _cnapi_parse_str($data, 'address', $xml, '@address');
  $latlng = explode(';', _cnapi_xpath_str($xml, '@latlng'));
  if (count($latlng) == 2) {
    $data['gis']['lat'] = $latlng[0];
    $data['gis']['lng'] = $latlng[1];
  }
  
  // $data['calendarsummary']
  _cnapi_parse_str($data, 'calendarsummary', $xml, '@calendarsummary');
  
  // $data['headingid']
  _cnapi_parse_str($data, 'headingid', $xml, '@headingid');
  if (isset($data['headingid'])) {
    $data['headingid'] = explode(';', $data['headingid']);
  }
  
  // $data['locationid']
  _cnapi_parse_str($data, 'locationid', $xml, '@locationid');
  
  // $data['available_to']
  _cnapi_parse_str($data, 'available_to', $xml, '@available_to');
  if (isset($data['available_to'])) {
    $data['available_to'] = strtotime($data['available_to']); // @todo strtotime doesn't work
  }
  
  // $data['agefrom']
  _cnapi_parse_str($data, 'agefrom', $xml, '@agefrom');
  
  // $data['performers']
  _cnapi_parse_str($data, 'performers', $xml, '@performers');
  if (isset($data['performers'])) {
    $data['performers'] = explode(';', $data['performers']);
  }
}

function _cnapi_parse_production_list_item(&$data, $xml) {
  $data = array();
  
  // $data['cdbid']
  _cnapi_parse_str($data, 'cdbid', $xml, '@cdbid');
  
  // $data['title']
  _cnapi_parse_str($data, 'title', $xml, '@title');
  
  // $data['shortdescription']
  _cnapi_parse_str($data, 'shortdescription', $xml, '@shortdescription');
  
  // $data['thumbnail']
  _cnapi_parse_str($data, 'thumbnail', $xml, '@thumbnail');
  
  // $data['agefrom']
  _cnapi_parse_str($data, 'agefrom', $xml, '@agefrom');
  
  // $data['performers']
  _cnapi_parse_str($data, 'performers', $xml, '@performers');
  if (isset($data['performers'])) {
    $data['performers'] = explode(';', $data['performers']);
  }
  
  return $data;
}

function _cnapi_parse_actor_list_item(&$data, $xml) {
  $data = array();
  
  // $data['cdbid']
  _cnapi_parse_str($data, 'cdbid', $xml, '@cdbid');
  
  // $data['title']
  _cnapi_parse_str($data, 'title', $xml, '@title');
  
  // $data['shortdescription']
  _cnapi_parse_str($data, 'shortdescription', $xml, '@shortdescription');
  
  // $data['thumbnail']
  _cnapi_parse_str($data, 'thumbnail', $xml, '@thumbnail');
  
  // $data['city']
  _cnapi_parse_str($data, 'city', $xml, '@city');
  
  // $data['zip']
  _cnapi_parse_str($data, 'zip', $xml, '@zip');
  
  // $data['address']
  _cnapi_parse_str($data, 'address', $xml, '@address');
  $latlng = explode(';', _cnapi_xpath_str($xml, '@latlng'));
  if (count($latlng) == 2) {
    $data['gis']['lat'] = $latlng[0];
    $data['gis']['lng'] = $latlng[1];
  }
  
  return $data;
}

function _cnapi_parse_event(&$data, $field = NULL, $xml) { // @todo starts at wrong level
  $event = array();

  // $event['cdbid']
  _cnapi_parse_str($event, 'cdbid', $xml, '@cdbid');
  
  // $event['detail'][...]
  _cnapi_parse_event_details($event, 'detail', $xml);
  
  // $event['categories'][...]
  _cnapi_parse_categories($event, 'categories', $xml);
  
  // $event['keywords']
  _cnapi_parse_str($event, 'keywords', $xml, 'c:keywords');
  if (isset($event['keywords'])) {
    $event['keywords'] = explode(';', $data['keywords']);
  }
  
  // $event['headings']
  _cnapi_parse_str($event, 'headings', $xml, 'c:headings/c:heading/@id', TRUE);
  
  // $event['agefrom']
  _cnapi_parse_str($event, 'agefrom', $xml, '@agefrom');
  
  // $event['calendar'][...]
  _cnapi_parse_calendar($event, 'calendar', $xml);
  
  // $event['contactinfo'][...]
  _cnapi_parse_contactinfo($event, 'contactinfo', $xml);
  
  // $event['location']['address'][...]
  if ($xml_location = _cnapi_xpath($xml, 'c:location')) {
    _cnapi_parse_address($event['location'], 'address', array_shift($xml_location));
  }
  
  // $event['location']['actor'][...]
  if ($xml_location_actor = _cnapi_xpath($xml, 'c:location/c:actor')) {
    _cnapi_parse_actor($event['location'], 'actor', array_shift($xml_location_actor));
  }
  
  // $event['organiser'][...]
  if ($xml_organiser = _cnapi_xpath($xml, 'c:organiser/c:actor')) {
    _cnapi_parse_actor($event, 'organiser', array_shift($xml_organiser));
  }
  
  // $event['related_events'][...]
  _cnapi_parse_related_events($event, 'related_events', $xml);
  
  if ($xml_event_relations = _cnapi_xpath($xml, 'c:eventrelations')) {
    $xml_event_relations = array_shift($xml_event_relations);
    
    // $event['related_production'][...]
    _cnapi_parse_related_production($event, 'related_production', $xml_event_relations);
    
    // $event['parent_event'][...]
    _cnapi_parse_parent_event($event, 'parent_event', $xml_event_relations);
  }
  
  if (!empty($event)) {
    // caching this object
    _cnapi_cache_event($event);
    
    // setting field on object
    if ($field) {
      $data[$field] = $event;
    }
    else {
      $data = $event;
    }
  }
}

function _cnapi_parse_production(&$data, $field = NULL, $xml) { // @todo starts at wrong level
  $production = array();

  // $production['cdbid']
  _cnapi_parse_str($production, 'cdbid', $xml, '@cdbid');
  
  // $production['detail'][...]
  _cnapi_parse_production_details($production, 'detail', $xml);
  
  // $production['categories'][...]
  _cnapi_parse_categories($production, 'categories', $xml);
  
  // $production['keywords']
  _cnapi_parse_str($production, 'keywords', $xml, 'c:keywords');
  if (isset($production['keywords'])) {
    $production['keywords'] = explode(';', $production['keywords']);
  }
  
  // $production['agefrom']
  _cnapi_parse_str($production, 'agefrom', $xml, '@agefrom');
  
  // $production['related_events'][...]
  _cnapi_parse_related_events($production, 'related_events', $xml);
  
  if (!empty($production)) {
    // caching this object
    _cnapi_cache_production($production);
  
    // setting field on object
    if ($field) {
      $data[$field] = $production;
    }
    else {
      $data = $production;
    }
  }
}

function _cnapi_parse_actor(&$data, $field = NULL, $xml) { // @todo starts at wrong level
  $actor = array();

  // $actor['cdbid']
  _cnapi_parse_str($actor, 'cdbid', $xml, '@cdbid');
  
  // $actor['detail'][...]
  _cnapi_parse_actor_details($actor, 'detail', $xml);
  
  // $actor['categories'][...]
  _cnapi_parse_categories($actor, 'categories', $xml);
  
  // $actor['keywords']
  _cnapi_parse_str($actor, 'keywords', $xml, 'c:keywords');
  if (isset($actor['keywords'])) {
    $actor['keywords'] = explode(';', $actor['keywords']);
  }
  
  // $actor['contactinfo'][...]
  _cnapi_parse_contactinfo($actor, 'contactinfo', $xml);

  if (!empty($actor)) {
    // caching this object
    _cnapi_cache_actor($actor);
  
    // setting field on object
    if ($field) {
      $data[$field] = $actor;
    }
    else {
      $data = $actor;
    }
  }
}

function _cnapi_cache_event($event) {
  // constructing the $request object for generating the cache id
  $request = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $event['cdbid']));
  
  if (_cnapi_xpath($xml, 'c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'list';
  }
  elseif (_cnapi_xpath($xml, 'c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'detail';
  }
  
  // caching the event
  _cnapi_cache_set($request, $event);
}

function _cnapi_cache_production($production) {
  // constructing the $request object for generating the cache id
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $production['cdbid']));
  
  if (_cnapi_xpath($xml, 'c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'list';
  }
  elseif (_cnapi_xpath($xml, 'c:relatedevents/c:list/c:item')) {
    $request['query']['relatedevents'] = 'detail';
  }
  
  // caching the production
  _cnapi_cache_set($request, $production);
}

function _cnapi_cache_production($production) {
  // constructing the $request object for generating the cache id
  $request = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $actor['cdbid']));
  
  // caching the actor
  _cnapi_cache_set($request, $actor);
}

function _cnapi_parse_event_details(&$data, $field, $xml) {
  $details = array();

  if ($languages = _cnapi_xpath_str($xml, 'c:eventdetails/c:eventdetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = array_shift(_cnapi_xpath($xml, 'c:eventdetails/c:eventdetail[@lang="' . $language . '"]'));
      _cnapi_parse_event_detail($details, $language, $xml_detail);
    }
  }

  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_event_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();

  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  _cnapi_parse_str($detail, 'calendarsummary', $xml, 'c:calendarsummary');
  _cnapi_parse_performers($detail, 'performers', $xml);
  _cnapi_parse_media($detail, 'media', $xml);
  _cnapi_parse_price($detail, 'price', $xml);

  if (!empty($detail)) {
    $data[$field] = $detail;
  }
}

function _cnapi_parse_production_details(&$data, $field, $xml) {
  $details = array();

  if ($languages = _cnapi_xpath_str($xml, 'c:productiondetails/c:productiondetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = array_shift(_cnapi_xpath($xml, 'c:productiondetails/c:productiondetail[@lang="' . $language . '"]'));
      _cnapi_parse_production_detail($details, $language, $xml_detail);
    }
  }

  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_production_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();

  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  _cnapi_parse_performers($detail, 'performers', $xml);
  _cnapi_parse_media($detail, 'media', $xml);

  if (!empty($detail)) {
    $data[$field] = $detail;
  }
}

function _cnapi_parse_actor_details(&$data, $field, $xml) {
  $details = array();

  if ($languages = _cnapi_xpath_str($xml, 'c:actordetails/c:actordetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = array_shift(_cnapi_xpath($xml, 'c:actordetails/c:actordetail[@lang="' . $language . '"]'));
      _cnapi_parse_actor_detail($details, $language, $xml_detail);
    }
  }

  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_actor_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();

  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  _cnapi_parse_media($detail, 'media', $xml);

  if (!empty($detail)) {
    $data[$field] = $detail;
  }
}

function _cnapi_parse_related_production(&$data, $field, $xml) {
  if ($xml = array_shift(_cnapi_xpath($xml, 'c:relatedproduction'))) {
    _cnapi_parse_str($data[$field], 'cdbid', $xml, '@cdbid');
    $data[$field]['title'] = strval($xml);
  }
}

function _cnapi_parse_parent_event(&$data, $field, $xml) {
  if ($xml = array_shift(_cnapi_xpath($xml, 'c:parentevent'))) {
    _cnapi_parse_str($data[$field], 'cdbid', $xml, '@cdbid');
    $data[$field]['title'] = strval($xml);
  }
}

function _cnapi_parse_related_events(&$data, $field, $xml) {
  $related_events = array();
  
  if (!($xml = _cnapi_xpath($xml, 'c:relatedevents'))) {
    return;
  }
  
  $xml = array_shift($xml);
  
  // @todo cleanup
  
  // related events as list_summary
  if ($items = _cnapi_xpath($xml, 'c:list/c:item')) {
    foreach ($items as $item) {
      $object = array();
      _cnapi_parse_event_list_item($object, $item);

      $related_events[] = $object;
    }
  }

  // related events as list_detail
  elseif ($items = _cnapi_xpath($xml, 'c:event')) {
    foreach ($items as $item) {
      $object = array();
      _cnapi_parse_event($object, NULL, $item);
      if (!empty($object)) {
        $related_events[] = $object;
      }
    }
  }

  // related events as list of cdbid
  else {
    $related_events = _cnapi_xpath_str($xml, 'c:id/@cdbid', TRUE);
  }
  
  if (!empty($related_events)) {
    $data[$field] = $related_events;
  }
}

function _cnapi_parse_calendar(&$data, $field, $xml) {
  // @todo implement this
}

function _cnapi_parse_contactinfo(&$data, $field, $xml) {
  $info = array();
  
  if (!($xml = _cnapi_xpath($xml, 'c:contactinfo'))) {
    return;
  }
  
  $xml = array_shift($xml);
  
  _cnapi_parse_address($info, 'address', $xml);

  // main
  $main = array();
  _cnapi_parse_str($main, 'mail', $xml, 'c:mail[@main="true"]');
  _cnapi_parse_str($main, 'phone', $xml, 'c:phone[@main="true" and @type="phone"]');
  _cnapi_parse_str($main, 'fax', $xml, 'c:phone[@main="true" and @type="fax"]');
  _cnapi_parse_str($main, 'url', $xml, 'c:url[@main="true"]');
  
  if (!empty($main)) {
    $info['main'] = $main;
  }

  // reservation
  $reservation = array();
  _cnapi_parse_str($reservation, 'mail', $xml, 'c:mail[@reservation="true"]');
  _cnapi_parse_str($reservation, 'phone', $xml, 'c:phone[@reservation="true" and @type="phone"]');
  _cnapi_parse_str($reservation, 'fax', $xml, 'c:phone[@reservation="true" and @type="fax"]');
  _cnapi_parse_str($reservation, 'url', $xml, 'c:url[@reservation="true"]');
  
  if (!empty($reservation)) {
    $info['reservation'] = $reservation;
  }

  // reservation
  $other = array();
  _cnapi_parse_str($other, 'mail', $xml, 'c:mail[not(@main="true" or @reservation="true")]', TRUE);
  _cnapi_parse_str($other, 'phone', $xml, 'c:phone[not(@main="true" or @reservation="true") and @type="phone"]', TRUE);
  _cnapi_parse_str($other, 'fax', $xml, 'c:phone[not(@main="true" or @reservation="true") and @type="fax"]', TRUE);
  _cnapi_parse_str($other, 'url', $xml, 'c:url[not(@main="true" or @reservation="true")]', TRUE);
  
  if (!empty($other)) {
    $info['other'] = $other;
  }
  
  if (!empty($info)) {
    $data[$field] = $info;
  }
}

function _cnapi_parse_categories(&$data, $field, $xml) {
  $categories = array();
  
  if ($types = _cnapi_xpath_str($xml, 'c:categories/c:category/@type', TRUE)) {
    foreach ($types as $type) {
      _cnapi_parse_str($categories, $type, $xml, 'c:categories/c:category[@type="' . $type . '"]/@catid', TRUE);
    }
  }
  
  if (!empty($categories)) {
    $data[$field] = $categories;
  }
}

function _cnapi_parse_address(&$data, $field, $xml) {
  _cnapi_parse_str($address, 'street', $xml, 'c:address/c:physical/c:street');
  _cnapi_parse_str($address, 'housenr', $xml, 'c:address/c:physical/c:housenr');
  _cnapi_parse_str($address, 'zipcode', $xml, 'c:address/c:physical/c:zipcode');
  _cnapi_parse_str($address, 'city', $xml, 'c:address/c:physical/c:city');
  _cnapi_parse_str($address, 'country', $xml, 'c:address/c:physical/c:country');
  
  $gis = array();
  _cnapi_parse_str($gis, 'lat', $xml, 'c:address/c:physical/c:gis/c:xcoordinate');
  _cnapi_parse_str($gis, 'lng', $xml, 'c:address/c:physical/c:gis/c:ycoordinate');
  if (!empty($gis)) {
    $address['gis'] = $gis;
  }
  
  // @improvement add cityid
    
  if (!empty($address)) {
    $data[$field] = $address;
  }
}

function _cnapi_parse_performers(&$data, $field, $xml) {
  $performers = array();
  
  // @todo clean this up
  if ($xml_performers = _cnapi_xpath($xml, 'c:performers/c:performer')) {
    foreach ($xml_performers as $xml_performer) {
      if ($xml_actor = _cnapi_xpath($xml_performer, 'c:actor')) {
        $xml_actor = array_shift($xml_actor);
        
        $actor = array();
        _cnapi_parse_actor($actor, NULL, $xml_actor);
        
        if (!empty($actor)) {
          $performer = array();
          _cnapi_parse_str($performer, 'role', $xml_performer, 'c:role');
          $performer['name'] = $actor['detail']['nl']['title'];
          $performer['actor'] = $actor;
          
          $performers[] = $performer;
        }
      }
      elseif ($xml_label = _cnapi_xpath($xml_performer, 'c:label')) {
        $performer = array();
        
        _cnapi_parse_str($performer, 'role', $xml_performer, 'c:role');
        _cnapi_parse_str($performer, 'name', $xml_performer, 'c:label');
        
        if (!empty($performer)) {
          $performers[] = $performer;
        }
      }
    }
  }
  
  if (!empty($performers)) {
    $data[$field] = $performers;
  }
}

function _cnapi_parse_media(&$data, $field, $xml) {
  $files = array();

  if ($xml_files = _cnapi_xpath($xml, 'c:media/c:file')) {
    foreach ($xml_files as $xml_file) {
      $file = array();
      
      _cnapi_parse_str($file, 'copyright', $xml_file, 'c:copyright');
      _cnapi_parse_str($file, 'filename', $xml_file, 'c:filename');
      _cnapi_parse_str($file, 'filetype', $xml_file, 'c:filetype');
      _cnapi_parse_str($file, 'mediatype', $xml_file, 'c:mediatype');
      _cnapi_parse_str($file, 'title', $xml_file, 'c:title');
      _cnapi_parse_str($file, 'hlink', $xml_file, 'c:hlink');
      _cnapi_parse_str($file, 'plaintext', $xml_file, 'c:plaintext');
      
      if (!empty($file)) {
        $files[] = $file;
      }
      
      // @todo mark main file
    }
  }
  
  if (!empty($files)) {
    $data[$field] = $files;
  }
}

function _cnapi_parse_price(&$data, $field, $xml) {
  $price = array();
  
  _cnapi_parse_float($price, 'value', $xml, 'c:price/c:pricevalue');
  _cnapi_parse_str($price, 'description', $xml, 'c:price/c:pricedescription');
  
  if (!empty($price)) {
    $data[$field] = $price;
  }
}