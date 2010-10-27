<?php
// $Id$

function _cnapi_parse($request, $xml) {
  $data = array();

  // we don't care for namespaces here
  $xml = str_replace(' xmlns=', ' ns=', $xml);
  
  // trying to parse the xml
  try {
    $xml = new SimpleXMLElement($xml);
  }
  catch (Exception $e) {
    watchdog('cnapi', 'An error occured while parsing url for !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
    return FALSE;
  }
  
  // detail
  if ($request['action'] == 'detail') {
    $xpath = sprintf('/cdbxml/%ss/%s', $request['type'], $request['type']);
    if ($xml_object = $xml->xpath($xpath)) {
      call_user_func('_cnapi_parse_' . $request['type'], &$data, NULL, reset($xml_object));
    }
  }
  
  // list_detail
  elseif ($request['action'] == 'list_detail') {
    // $data['total']
    $data['total'] = _cnapi_xpath_str($xml, 'nofrecords');
    
    // $data['data']
    $data['data'] = array();
    
    $xpath = sprintf('/cdbxml/%ss/%s', $request['type'], $request['type']);
    if ($xml_objects = $xml->xpath($xpath)) {
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
    // $data['total']
    $data['total'] = _cnapi_xpath_str($xml, 'nofrecords');
    
    // $data['data']
    $items_path = '/cdbxml/list/item';
    if ($request['type'] == 'production') {
      $items_path = '/cdbxml/production/item';
    }

    $xml_items = $xml->xpath($items_path);
    foreach ($xml_items as $xml_item) {
      $object = array();
      call_user_func('_cnapi_parse_' . $request['type'] . '_list_item', &$object, $xml_item);

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

function _cnapi_parse_event(&$data, $field = NULL, $xml) {
  $event = array();

  // $event['cdbid']
  _cnapi_parse_str($event, 'cdbid', $xml, '@cdbid');
  
  // $event['detail'][...]
  _cnapi_parse_event_details($event, 'detail', $xml);
  
  // $event['categories'][...]
  _cnapi_parse_categories($event, 'categories', $xml);
  
  // $event['keywords']
  _cnapi_parse_str($event, 'keywords', $xml, 'keywords');
  if (isset($event['keywords'])) {
    $event['keywords'] = explode(';', $event['keywords']);
  }
  
  // $event['headings']
  _cnapi_parse_str($event, 'headings', $xml, 'headings/heading/@id', TRUE);
  
  // $event['agefrom']
  _cnapi_parse_str($event, 'agefrom', $xml, '@agefrom');
  
  // $event['calendar'][...]
  _cnapi_parse_calendar($event, 'calendar', $xml);
  
  // $event['contactinfo'][...]
  _cnapi_parse_contactinfo($event, 'contactinfo', $xml);
  
  // $event['location']['address'][...]
  if ($xml_location = $xml->xpath('location')) {
    _cnapi_parse_address($event['location'], 'address', reset($xml_location));
  }
  
  // $event['location']['actor'][...]
  if ($xml_location_actor = $xml->xpath('location/actor')) {
    _cnapi_parse_actor($event['location'], 'actor', reset($xml_location_actor));
  }
  
  // $event['organiser'][...]
  if ($xml_organiser = $xml->xpath('organiser/actor')) {
    _cnapi_parse_actor($event, 'organiser', reset($xml_organiser));
  }
  
  // $event['related_events'][...]
  _cnapi_parse_related_events($event, 'related_events', $xml);

  if ($xml_event_relations = $xml->xpath('eventrelations')) {
    $xml_event_relations = reset($xml_event_relations);
    
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

function _cnapi_parse_production(&$data, $field = NULL, $xml) {
  $production = array();

  // $production['cdbid']
  _cnapi_parse_str($production, 'cdbid', $xml, '@cdbid');
  
  // $production['detail'][...]
  _cnapi_parse_production_details($production, 'detail', $xml);
  
  // $production['categories'][...]
  _cnapi_parse_categories($production, 'categories', $xml);
  
  // $production['keywords']
  _cnapi_parse_str($production, 'keywords', $xml, 'keywords');
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

function _cnapi_parse_actor(&$data, $field = NULL, $xml) {
  $actor = array();

  // $actor['cdbid']
  _cnapi_parse_str($actor, 'cdbid', $xml, '@cdbid');
  
  // $actor['detail'][...]
  _cnapi_parse_actor_details($actor, 'detail', $xml);
  
  // $actor['categories'][...]
  _cnapi_parse_categories($actor, 'categories', $xml);
  
  // $actor['keywords']
  _cnapi_parse_str($actor, 'keywords', $xml, 'keywords');
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
  
  if ($xml->xpath('relatedevents/list/item')) {
    $request['query']['relatedevents'] = 'list';
  }
  elseif ($xml->xpath('relatedevents/list/item')) {
    $request['query']['relatedevents'] = 'detail';
  }
  
  // caching the event
  _cnapi_cache_set($request, $event);
}

function _cnapi_cache_production($production, $xml) {
  // constructing the $request object for generating the cache id
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $production['cdbid']));
  
  if ($xml->xpath('relatedevents/list/item')) {
    $request['query']['relatedevents'] = 'list';
  }
  elseif ($xml->xpath('relatedevents/list/item')) {
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

  if ($languages = _cnapi_xpath_str($xml, 'eventdetails/eventdetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = reset($xml->xpath('eventdetails/eventdetail[@lang="' . $language . '"]'));
      _cnapi_parse_event_detail($details, $language, $xml_detail);
    }
  }

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_event_detail(&$data, $field, $xml) {
  $detail = array();
  
  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'longdescription');
  
  // $detail['calendarsummary']
  _cnapi_parse_str($detail, 'calendarsummary', $xml, 'calendarsummary');
  
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

  if ($languages = _cnapi_xpath_str($xml, 'productiondetails/productiondetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = reset($xml->xpath('productiondetails/productiondetail[@lang="' . $language . '"]'));
      _cnapi_parse_production_detail($details, $language, $xml_detail);
    }
  }

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_production_detail(&$data, $field, $xml) {
  $detail = array();

  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'longdescription');
  
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

  if ($languages = _cnapi_xpath_str($xml, 'actordetails/actordetail/@lang', TRUE)) {
    foreach ($languages as $language) {
      $xml_detail = reset($xml->xpath('actordetails/actordetail[@lang="' . $language . '"]'));
      _cnapi_parse_actor_detail($details, $language, $xml_detail);
    }
  }

  // adding data to requesting object
  if (!empty($details)) {
    $data[$field] = $details;
  }
}

function _cnapi_parse_actor_detail(&$data, $field, $xml) {
  $detail = array();

  // $detail['title']
  _cnapi_parse_str($detail, 'title', $xml, 'title');
  
  // $detail['shortdescription']
  _cnapi_parse_str($detail, 'shortdescription', $xml, 'shortdescription');
  
  // $detail['longdescription']
  _cnapi_parse_str($detail, 'longdescription', $xml, 'longdescription');
  
  // $detail['media'][...]
  _cnapi_parse_media($detail, 'media', $xml);

  // adding data to requesting object
  if (!empty($detail)) {
    $data[$field] = $detail;
  }
}

function _cnapi_parse_related_production(&$data, $field, $xml) {
  // $data[$field]['cdbid']
  _cnapi_parse_str($data[$field], 'cdbid', $xml, 'relatedproduction/@cdbid');

  // $data[$field]['title']
  _cnapi_parse_str($data[$field], 'title', $xml, 'relatedproduction');
  
  // removing empty related_production
  if (empty($data[$field])) {
    unset($data[$field]);
  }
}

function _cnapi_parse_parent_event(&$data, $field, $xml) {
  // $data[$field]['cdbid']
  _cnapi_parse_str($data[$field], 'cdbid', $xml, 'parentevent/@cdbid');
  
  // $data[$field]['title']
  _cnapi_parse_str($data[$field], 'title', $xml, 'parentevent');
  
  // removing empty parent_event
  if (empty($data[$field])) {
    unset($data[$field]);
  }
}

function _cnapi_parse_related_events(&$data, $field, $xml) {
  $related_events = array();
  
  // @todo cleanup
  
  // related events as list_summary
  if ($items = $xml->xpath('relatedevents/list/item')) {
    foreach ($items as $item) {
      $object = array();
      _cnapi_parse_event_list_item($object, $item);
      if (!empty($object)) {
        $related_events[] = $object;
      }
    }
  }

  // related events as list_detail
  elseif ($items = $xml->xpath('relatedevents/event')) {
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
    $related_events = _cnapi_xpath_str($xml, 'relatedevents/id/@cdbid', TRUE);
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
  _cnapi_parse_address($info, 'contactinfo/address', $xml);

  // $info['main']['mail']
  _cnapi_parse_str($info['main'], 'mail', $xml, 'contactinfo/mail[@main="true"]');
  
  // $info['main']['phone']
  _cnapi_parse_str($info['main'], 'phone', $xml, 'contactinfo/phone[@main="true" and @type="phone"]');
  
  // $info['main']['fax']
  _cnapi_parse_str($info['main'], 'fax', $xml, 'contactinfo/phone[@main="true" and @type="fax"]');
  
  // $info['main']['url']
  _cnapi_parse_str($info['main'], 'url', $xml, 'contactinfo/url[@main="true"]');

  // $info['reservation']['mail']
  _cnapi_parse_str($info['reservation'], 'mail', $xml, 'contactinfo/mail[@reservation="true"]');
  
  // $info['reservation']['phone']
  _cnapi_parse_str($info['reservation'], 'phone', $xml, 'contactinfo/phone[@reservation="true" and @type="phone"]');
  
  // $info['reservation']['fax']
  _cnapi_parse_str($info['reservation'], 'fax', $xml, 'contactinfo/phone[@reservation="true" and @type="fax"]');
  
  // $info['reservation']['url']
  _cnapi_parse_str($info['reservation'], 'url', $xml, 'contactinfo/url[@reservation="true"]');

  // $info['other']['mail']
  _cnapi_parse_str($info['other'], 'mail', $xml, 'contactinfo/mail[not(@main="true" or @reservation="true")]', TRUE);
  
  // $info['other']['phone']
  _cnapi_parse_str($info['other'], 'phone', $xml, 'contactinfo/phone[not(@main="true" or @reservation="true") and @type="phone"]', TRUE);
  
  // $info['other']['fax']
  _cnapi_parse_str($info['other'], 'fax', $xml, 'contactinfo/phone[not(@main="true" or @reservation="true") and @type="fax"]', TRUE);
  
  // $info['other']['url']
  _cnapi_parse_str($info['other'], 'url', $xml, 'contactinfo/url[not(@main="true" or @reservation="true")]', TRUE);
  
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
  
  if ($types = _cnapi_xpath_str($xml, 'categories/category/@type', TRUE)) {
    foreach ($types as $type) {
      _cnapi_parse_str($categories, $type, $xml, 'categories/category[@type="' . $type . '"]/@catid', TRUE);
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
  _cnapi_parse_str($address, 'street', $xml, 'address/physical/street');
  
  // $address['housenr']
  _cnapi_parse_str($address, 'housenr', $xml, 'address/physical/housenr');
  
  // $address['zipcode']
  _cnapi_parse_str($address, 'zipcode', $xml, 'address/physical/zipcode');
  
  // $address['city']
  _cnapi_parse_str($address, 'city', $xml, 'address/physical/city');
  
  // $address['country']
  _cnapi_parse_str($address, 'country', $xml, 'address/physical/country');
  
  // $address['gis'][...]
  $gis = array();
  _cnapi_parse_str($gis, 'lat', $xml, 'address/physical/gis/xcoordinate');
  _cnapi_parse_str($gis, 'lng', $xml, 'address/physical/gis/ycoordinate');
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
  if ($xml_performers = $xml->xpath('performers/performer')) {
    foreach ($xml_performers as $xml_performer) {
      // performer as an actor
      if ($xml_actor = $xml->xpath('actor')) {
        $actor = array();
        _cnapi_parse_actor($actor, NULL, reset($xml_actor));
        
        if (!empty($actor)) {
          $performer = array();
          _cnapi_parse_str($performer, 'role', $xml_performer, 'role');
          $performer['name'] = $actor['detail']['nl']['title'];
          $performer['actor'] = $actor;
          
          $performers[] = $performer;
        }
      }
      
      // peformer as a label
      elseif ($xml->xpath('label')) {
        $performer = array();
        
        _cnapi_parse_str($performer, 'role', $xml_performer, 'role');
        _cnapi_parse_str($performer, 'name', $xml_performer, 'label');
        
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

  if ($xml_files = $xml->xpath('media/file')) {
    foreach ($xml_files as $xml_file) {
      $file = array();
      
      // $file['copyright']
      _cnapi_parse_str($file, 'copyright', $xml_file, 'copyright');
      
      // $file['filename']
      _cnapi_parse_str($file, 'filename', $xml_file, 'filename');
      
      // $file['filetype']
      _cnapi_parse_str($file, 'filetype', $xml_file, 'filetype');
      
      // $file['mediatype']
      _cnapi_parse_str($file, 'mediatype', $xml_file, 'mediatype');
      
      // $file['title']
      _cnapi_parse_str($file, 'title', $xml_file, 'title');
      
      // $file['hlink']
      _cnapi_parse_str($file, 'hlink', $xml_file, 'hlink');
      
      // $file['plaintext']
      _cnapi_parse_str($file, 'plaintext', $xml_file, 'plaintext');
      
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
  _cnapi_parse_float($price, 'value', $xml, 'price/pricevalue');
  
  // $data['description']
  _cnapi_parse_str($price, 'description', $xml, 'price/pricedescription');
  
  // adding data to requesting object
  if (!empty($price)) {
    $data[$field] = $price;
  }
}