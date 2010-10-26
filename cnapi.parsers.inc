<?php
// $Id$

function _cnapi_parse($request, $xml) {
  $data = array();

  // trying to parse the xml
  $xml = new SimpleXMLElement($xml);
  
  if (!$xml) {
    // @todo log this
  }
  
  // registering the default namespace
  _cnapi_register_default_namespace($xml, 'c');
  
  // detail
  if ($request['action'] == 'detail') {
    $xpath = sprintf('/c:cdbxml/c:%ss/c:%s', $request['type'], $request['type']);
    if ($xml_object = _cnapi_xpath($xml, $xpath)) {
      $xml_object = array_shift($xml_object);
      call_user_func('_cnapi_parse_' . $request['type'], &$data, NULL, $xml_object);
    }
  }
  
  // list_detail
  elseif ($request['action'] == 'list_detail') {
    // $data['total]
    $data['total'] = _cnapi_xpath_str($xml, 'c:nofrecords');
    
    // $data['data]
    $data['data'] = array();
    
    $xpath = sprintf('/c:cdbxml/c:%ss/c:%s', $request['type'], $request['type']);
    if ($xml_objects = _cnapi_xpath($xml, $xpath)) {
      foreach ($xml_objects as $xml_object) {
        $object = array();
        call_user_func('_cnapi_parse_' . $request['type'], &$object, NULL, $xml_object);
        if (!empty($object)) {
          $data['data'][] = $object;
        }
      }
    }
  }
  
  // list_summary
  elseif ($request['action'] == 'list_summary') {
    // $data['total]
    $data['total'] = _cnapi_xpath_str($xml, 'c:nofrecords');
    
    // $data['data]
    $items_path = '/c:cdbxml/c:list/c:item';
    if ($request['type'] == 'production') {
      $items_path = '/c:cdbxml/c:production/c:item';
    }
    
    foreach (_cnapi_xpath($xml, $items_path) as $item) {
      $object = array();
      call_user_func('_cnapi_parse_' . $request['type'] . '_list_item', &$object, $item);

      $data['data'][] = $object;
    }
  }
  
  return $data;
}

function _cnapi_parse_event_list_item(&$data, $xml) {
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
}

function _cnapi_parse_actor_list_item(&$data, $xml) {
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
    _cnapi_cache_event($event, $xml);

    // adding data to requesting object
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
    _cnapi_cache_production($production, $xml);
  
    // adding data to requesting object
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
    _cnapi_cache_actor($actor, $xml);
  
    // adding data to requesting object
    if ($field) {
      $data[$field] = $actor;
    }
    else {
      $data = $actor;
    }
  }
}

function _cnapi_cache_event($event, $xml) {
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

function _cnapi_cache_production($production, $xml) {
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

function _cnapi_cache_actor($actor, $xml) {
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

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_event_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();
  
  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  
  // $detail['calendarsummary']
  _cnapi_parse_str($detail, 'calendarsummary', $xml, 'c:calendarsummary');
  
  // $detail['performers'][...]
  _cnapi_parse_performers($detail, 'performers', $xml);
  
  // $detail['media'][...]
  _cnapi_parse_media($detail, 'media', $xml);
  
  // $detail['price'][...]
  _cnapi_parse_price($detail, 'price', $xml);

  // adding data to requesting object
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

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_production_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();

  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  
  // $detail['performers'][...]
  _cnapi_parse_performers($detail, 'performers', $xml);
  
  // $detail['media'][...]
  _cnapi_parse_media($detail, 'media', $xml);

  // adding data to requesting object
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

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_actor_detail(&$data, $field, $xml) { // @todo starts at wrong level
  $detail = array();

  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'c:title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'c:shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'c:longdescription');
  
  // $detail['media'][...]
  _cnapi_parse_media($detail, 'media', $xml);

  // adding data to requesting object
  if (!empty($detail)) {
    $data[$field] = $detail;
  }
}

function _cnapi_parse_related_production(&$data, $field, $xml) {
  // $data[$field]['cdbid']
  _cnapi_parse_str($data[$field], 'cdbid', $xml, 'c:relatedproduction/@cdbid');

  // $data[$field]['title']
  _cnapi_parse_str($data[$field], 'title', $xml, 'c:relatedproduction');
  
  // removing empty related_production
  if (empty($data[$field])) {
    unset($data[$field]);
  }
}

function _cnapi_parse_parent_event(&$data, $field, $xml) {
  // $data[$field]['cdbid']
  _cnapi_parse_str($data[$field], 'cdbid', $xml, 'c:parentevent/@cdbid');
  
  // $data[$field]['title']
  _cnapi_parse_str($data[$field], 'title', $xml, 'c:parentevent');
  
  // removing empty parent_event
  if (empty($data[$field])) {
    unset($data[$field]);
  }
}

function _cnapi_parse_related_events(&$data, $field, $xml) {
  $related_events = array();
  
  // @todo cleanup
  
  // related events as list_summary
  if ($items = _cnapi_xpath($xml, 'c:relatedevents/c:list/c:item')) {
    foreach ($items as $item) {
      $object = array();
      _cnapi_parse_event_list_item($object, $item);
      if (!empty($object)) {
        $related_events[] = $object;
      }
    }
  }

  // related events as list_detail
  elseif ($items = _cnapi_xpath($xml, 'c:relatedevents/c:event')) {
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
    $related_events = _cnapi_xpath_str($xml, 'c:relatedevents/c:id/@cdbid', TRUE);
  }
  
  // adding data to requesting object
  if (!empty($related_events)) {
    $data[$field] = $related_events;
  }
}

function _cnapi_parse_calendar(&$data, $field, $xml) {
  // @todo implement this
}

function _cnapi_parse_contactinfo(&$data, $field, $xml) {
  $info = array(
    'main' => array(),
    'reservation' => array(),
    'other' => array(),
  );
  
  // $info['address'][...]
  _cnapi_parse_address($info, 'c:contactinfo/address', $xml);

  // $info['main']['mail']
  _cnapi_parse_str($info['main'], 'mail', $xml, 'c:contactinfo/c:mail[@main="true"]');
  
  // $info['main']['phone']
  _cnapi_parse_str($info['main'], 'phone', $xml, 'c:contactinfo/c:phone[@main="true" and @type="phone"]');
  
  // $info['main']['fax']
  _cnapi_parse_str($info['main'], 'fax', $xml, 'c:contactinfo/c:phone[@main="true" and @type="fax"]');
  
  // $info['main']['url']
  _cnapi_parse_str($info['main'], 'url', $xml, 'c:contactinfo/c:url[@main="true"]');

  // $info['reservation']['mail']
  _cnapi_parse_str($info['reservation'], 'mail', $xml, 'c:contactinfo/c:mail[@reservation="true"]');
  
  // $info['reservation']['phone']
  _cnapi_parse_str($info['reservation'], 'phone', $xml, 'c:contactinfo/c:phone[@reservation="true" and @type="phone"]');
  
  // $info['reservation']['fax']
  _cnapi_parse_str($info['reservation'], 'fax', $xml, 'c:contactinfo/c:phone[@reservation="true" and @type="fax"]');
  
  // $info['reservation']['url']
  _cnapi_parse_str($info['reservation'], 'url', $xml, 'c:contactinfo/c:url[@reservation="true"]');

  // $info['other']['mail']
  _cnapi_parse_str($info['other'], 'mail', $xml, 'c:contactinfo/c:mail[not(@main="true" or @reservation="true")]', TRUE);
  
  // $info['other']['phone']
  _cnapi_parse_str($info['other'], 'phone', $xml, 'c:contactinfo/c:phone[not(@main="true" or @reservation="true") and @type="phone"]', TRUE);
  
  // $info['other']['fax']
  _cnapi_parse_str($info['other'], 'fax', $xml, 'c:contactinfo/c:phone[not(@main="true" or @reservation="true") and @type="fax"]', TRUE);
  
  // $info['other']['url']
  _cnapi_parse_str($info['other'], 'url', $xml, 'c:contactinfo/c:url[not(@main="true" or @reservation="true")]', TRUE);
  
  // removing empty contactinfo types
  foreach ($info as $type => $value) {
    if (empty($info[$type])) {
      unset($info[$type]);
    }
  }
  
  // adding data to requesting object
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
  
  // adding data to requesting object
  if (!empty($categories)) {
    $data[$field] = $categories;
  }
}

function _cnapi_parse_address(&$data, $field, $xml) {
  $address = array();
  
  // $address['street']
  _cnapi_parse_str($address, 'street', $xml, 'c:address/c:physical/c:street');
  
  // $address['housenr']
  _cnapi_parse_str($address, 'housenr', $xml, 'c:address/c:physical/c:housenr');
  
  // $address['zipcode']
  _cnapi_parse_str($address, 'zipcode', $xml, 'c:address/c:physical/c:zipcode');
  
  // $address['city']
  _cnapi_parse_str($address, 'city', $xml, 'c:address/c:physical/c:city');
  
  // $address['country']
  _cnapi_parse_str($address, 'country', $xml, 'c:address/c:physical/c:country');
  
  // $address['gis'][...]
  $gis = array();
  _cnapi_parse_str($gis, 'lat', $xml, 'c:address/c:physical/c:gis/c:xcoordinate');
  _cnapi_parse_str($gis, 'lng', $xml, 'c:address/c:physical/c:gis/c:ycoordinate');
  if (!empty($gis)) {
    $address['gis'] = $gis;
  }
  
  // @improvement add cityid
  
  // adding data to requesting object
  if (!empty($address)) {
    $data[$field] = $address;
  }
}

function _cnapi_parse_performers(&$data, $field, $xml) {
  $performers = array();
  
  // @todo clean this up
  if ($xml_performers = _cnapi_xpath($xml, 'c:performers/c:performer')) {
    foreach ($xml_performers as $xml_performer) {
      // performer as an actor
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
      
      // peformer as a label
      elseif (_cnapi_xpath($xml_performer, 'c:label')) {
        $performer = array();
        
        _cnapi_parse_str($performer, 'role', $xml_performer, 'c:role');
        _cnapi_parse_str($performer, 'name', $xml_performer, 'c:label');
        
        // only adding performer if at least one field is available
        if (!empty($performer)) {
          $performers[] = $performer;
        }
      }
    }
  }
  
  // adding data to requesting object
  if (!empty($performers)) {
    $data[$field] = $performers;
  }
}

function _cnapi_parse_media(&$data, $field, $xml) {
  $files = array();

  if ($xml_files = _cnapi_xpath($xml, 'c:media/c:file')) {
    foreach ($xml_files as $xml_file) {
      $file = array();
      
      // $file['copyright']
      _cnapi_parse_str($file, 'copyright', $xml_file, 'c:copyright');
      
      // $file['filename']
      _cnapi_parse_str($file, 'filename', $xml_file, 'c:filename');
      
      // $file['filetype']
      _cnapi_parse_str($file, 'filetype', $xml_file, 'c:filetype');
      
      // $file['mediatype']
      _cnapi_parse_str($file, 'mediatype', $xml_file, 'c:mediatype');
      
      // $file['title']
      _cnapi_parse_str($file, 'title', $xml_file, 'c:title');
      
      // $file['hlink']
      _cnapi_parse_str($file, 'hlink', $xml_file, 'c:hlink');
      
      // $file['plaintext']
      _cnapi_parse_str($file, 'plaintext', $xml_file, 'c:plaintext');
      
      // only adding the file if we have at least one field
      if (!empty($file)) {
        $files[] = $file;
      }
      
      // @todo mark main file
    }
  }
  
  // adding data to requesting object
  if (!empty($files)) {
    $data[$field] = $files;
  }
}

function _cnapi_parse_price(&$data, $field, $xml) {
  $price = array();
  
  // $price['value']
  _cnapi_parse_float($price, 'value', $xml, 'c:price/c:pricevalue');
  
  // $data['description']
  _cnapi_parse_str($price, 'description', $xml, 'c:price/c:pricedescription');
  
  // adding data to requesting object
  if (!empty($price)) {
    $data[$field] = $price;
  }
}