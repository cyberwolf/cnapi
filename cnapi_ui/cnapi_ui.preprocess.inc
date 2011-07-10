<?php

/**
 * Preprocess variables for cnapi-ui-list.tpl.php.
 *
 * @see cnapi-ui-list.tpl.php
 */
function template_preprocess_cnapi_ui_list(&$variables) {
  // no results text
  $variables['no_results'] = t('No results found.');
  
  if (isset($variables['request']['query']['query'])) {
    $query = $variables['request']['query']['query'];
    
    $request_query_remove = $variables['request'];
    unset($request_query_remove['query']['query']);
    $query_remove_link = cnapi_url_dp2dul('x', $request_query_remove);
    
    $query = '<strong>' . $query . ' ' . $query_remove_link . '</strong>';
  }
  
  // result info
  if (isset($query)) {
    $variables['result_info'] = format_plural($variables['total'], '@total result found for !query', '@total results found for !query', array('@total' => $variables['total'], '!query' => $query));
  }
  else {
    $variables['result_info'] = format_plural($variables['total'], '@total result found', '@total results found', array('@total' => $variables['total']));
  }
  
  // objects
  $variables['objects'] = array();
  foreach ($variables['items'] as $object) {
    $variables['objects'][] = theme('cnapi_ui_' . $variables['type'] . '_summary', array($variables['type'] => $object));
  }
  
  // sort links
  $sort_links = array();
  
  $request_sort = $variables['request'];

  $request_sort['query']['sort'] = 'date ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By date'), $request_sort);

  $request_sort['query']['sort'] = 'location ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By location'), $request_sort);

  $request_sort['query']['sort'] = 'title ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By title'), $request_sort);

  $variables['sort_links'] = theme('item_list', array('items' => $sort_links));

  // rss
  $url_rss = cnapi_ui_rss_feed_url_raw();
  $variables['rss'] = l(t('RSS'), $url_rss['path'], $url_rss['options']);
  
  // pager
  $variables['pager'] = theme('pager');
}

/**
 * Helper for preprocessing variables common between cnapi_ui_actor_summary, cnapi_ui_event_summary, cnapi_ui_project_summary.
 */
function _template_preprocess_cnapi_ui_object_summary(&$variables, $type) {
  $object = $variables[$type];

  // cdbid
  $variables['cdbid'] = $object['cdbid'];

  // title
  $variables['title'] = check_plain($object['title']);

  // object_url
  $request_detail = array($type => $object['cdbid'], 'title' => $object['title']);
  $object_url = cnapi_url_dp2dua($request_detail);
  $object_url['options']['absolute'] = $variables['url_absolute'];

  $variables['object_url'] = url($object_url['path'], $object_url['options']);

  // more_link
  $variables['more_link'] = l(t('More info'), $object_url['path'], $object_url['options']);

  // thumbnail
  $default_image = url(drupal_get_path('module', 'cnapi_ui') . '/img/default-image.jpg', array('absolute' => $variables['url_absolute']));
  $thumbnail_url = isset($object['thumbnail']) && !empty($object['thumbnail']) ? $object['thumbnail'] : $default_image;
  $thumbnail_img = theme('image', array('path' => $thumbnail_url, 'alt' => $variables['title']));
  $object_url['options']['html'] = TRUE;
  $variables['thumbnail'] = l($thumbnail_img, $object_url['path'], $object_url['options']);

  // shortdescription
  $variables['shortdescription'] = isset($object['shortdescription']) && !empty($object['shortdescription']) ? truncate_utf8(strip_tags($object['shortdescription']), 200, TRUE, TRUE) : NULL;
}

/**
 * Helper for preprocessing performers in cnapi_ui_event_summary, cnapi_ui_project_summary.
 */
function _template_preprocess_cnapi_ui_object_performers(&$variables, $type) {
  $object = $variables[$type];

  // performers
  $variables['performers'] = NULL;

  if (isset($object['performers'])) {
    $performers = array_slice($object['performers'], 0, 3);
    $variables['performers'] = implode(', ', $performers);

    if (count($object['performers']) > 3) {
      $variables['performers'] .= ',&nbsp;&hellip;';
    }
  }
}

/**
 * Helper for preprocessing performers in cnapi_ui_event_summary, cnapi_ui_project_summary.
 */
function _template_preprocess_cnapi_ui_object_vlieg(&$variables, $type) {
  $object = $variables[$type];
  
  // In case url_absolute was not passed, we default to FALSE.
  if (!isset($variables['url_absolute'])) {
    $variables['url_absolute'] = FALSE;
  }

  // vlieg image
  $vlieg_description = 'Hallo, ik ben Vlieg en ik wijs de weg naar leuke activiteiten voor kinderen. Meer info op www.vliegjemee.be!';

  $vlieg_image_url = url(drupal_get_path('module', 'cnapi_ui') . '/img/vlieg.jpg', array('absolute' => $variables['url_absolute']));
  $variables['vlieg_image'] = theme('image', array('path' => $vlieg_image_url, 'alt' => $vlieg_description, 'title' => $vlieg_description, 'attributes' => array('class' => 'vlieg-image')));

  // vlieg
  $variables['for_children'] = FALSE;
  
  // for_children
  if (isset($object['agefrom'])) {
    $variables['for_children'] = $object['agefrom'] < 12;;
  }
}

/**
 * Preprocess variables for cnapi-ui-actor.tpl.php.
 *
 * @see cnapi-ui-actor.tpl.php
 */
function template_preprocess_cnapi_ui_actor_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'actor');
}

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'event');

  $event = $variables['event'];

  // performers
  _template_preprocess_cnapi_ui_object_performers($variables, 'event');

  // vlieg
  _template_preprocess_cnapi_ui_object_vlieg($variables, 'event');

  // where
  $where = array();
  if (isset($event['location']) && !empty($event['location'])) {
    $where[] = check_plain($event['location']);
  }
  if (isset($event['city']) && !empty($event['city'])) {
    $where[] = check_plain($event['city']);
  }
  $variables['where'] = !empty($where) ? implode(', ', $where) : NULL;

  // when
  $variables['when'] = isset($event['calendarsummary']) ? $event['calendarsummary'] : NULL;
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
 *
 * @see cnapi-ui-production.tpl.php
 */
function template_preprocess_cnapi_ui_production_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'production');

  // performers
  _template_preprocess_cnapi_ui_object_performers($variables, 'production');

  // vlieg
  _template_preprocess_cnapi_ui_object_vlieg($variables, 'production');
}

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event(&$variables) {
  $event = $variables['event'];

  $detail = $event['detail']['nl'];

  // cdbid
  $variables['cdbid'] = $event['cdbid'];

  // title
  $variables['title'] = check_plain($detail['title']);
  
  // shortdescription
  $variables['shortdescription'] = isset($detail['shortdescription']) && !empty($detail['shortdescription']) ? strip_tags($detail['shortdescription']) : '';

  // longdescription
  $variables['longdescription'] = isset($detail['longdescription']) && !empty($detail['longdescription']) ? $detail['longdescription'] : '';
  
  // headings
  $variables['headings'] = array();
  
  $headings = cnapi_get_headings();
  if (isset($event['headings'])) {
    foreach ($event['headings'] as $i => $heading) {
      $heading_info = $headings[$heading];
      if (!cnapi_output_type_has_heading_groups() || (cnapi_output_type_has_heading_groups() && $heading_info['pid'] != 0)) {
        $variables['headings'][] = $heading_info['name'];
      }
    }
  }
  
  // images
  $variables['images'] = array();
  
  $media = isset($detail['media']['other']) ? $detail['media']['other'] : array();
  if (isset($detail['media']['main'])) {
    $media[] = $detail['media']['main'];
  }
  
  foreach ($media as $media_item) {
    $image = array();
    
    // title
    if (isset($media_item['title'])) {
      $image['title'] = $media_item['title'];
    }
    
    // image
    if (isset($media_item['hlink']) && in_array($media_item['mediatype'], array('photo', 'imageweb'))) {
      $vars = array('path' => $media_item['hlink'] . '?maxwidth=240');
      if (isset($image['title'])) {
        $vars['alt'] = $image['title'];
        $vars['title'] = $image['title'];
      }
      $image['image'] = theme('image', $vars);
    }
    
    // copyright
    if (isset($media_item['copyright'])) {
      $image['copyright'] = $media_item['copyright'];
    }
    
    if (isset($image['image'])) {
      $variables['images'][] = $image;
    }
  }
  
  // price
  $variables['price'] = '';
  if (isset($detail['price']['value'])) {
    $variables['price'] = $detail['price']['value'] == '0' ? 'Gratis' : '&euro; '. $detail['price']['value'];
    $variables['price'] .= isset($detail['price']['description']) ? ' (' . $detail['price']['description'] . ')' : '';
  }
  elseif (isset($detail['price']['description'])) {
    $variables['price'] = $detail['price']['description'];
  }

  // vlieg
  _template_preprocess_cnapi_ui_object_vlieg($variables, 'event');

  // when
  $variables['when'] = isset($detail['calendarsummary']) ? $detail['calendarsummary'] : '';

  // performers
  $variables['performers'] = '';
  if (isset($detail['performers'])) {
    $performers = array();
    foreach ($detail['performers'] as $performer) {
      $role = isset($performer['role']) ? ' (' . $performer['role'] . ')' : '';
      $performers[] = $performer['name'] . $role;
    }
    $variables['performers'] = implode(', ', $performers);
  }
  
  // location
  if (isset($event['location'])) {
    $location = $event['location'];
    
    // location address
    if (isset($location['address'])) {
      $address = $address_street = $address_city = array();
      
      if (isset($location['address']['street'])) $address_street[] =  $location['address']['street'];
      if (isset($location['address']['housenr'])) $address_street[] = $location['address']['housenr'];
      
      if (isset($location['address']['zipcode'])) $address_city[] = $location['address']['zipcode'];
      if (isset($location['address']['city'])) $address_city[] = $location['address']['city'];
      
      if (!empty($address_street)) $address[] = trim(implode(' ', $address_street));
      if (!empty($address_city))$address[] = trim(implode(' ', $address_city));
      
      if (!empty($address)) {
        $variables['location']['address'] = implode(', ', $address);
      }
    }
    
    // location title
    if (isset($location['actor']['detail']['nl']['title'])) {
      $variables['location']['title'] = $location['actor']['detail']['nl']['title'];
    }
    
    // location link
    if (isset($location['actor']['asset']) && $location['actor']['asset']) {
      $request_actor = array('actor' => $location['actor']['cdbid'], 'title' => $variables['location']['title']);
      $variables['location']['link'] = cnapi_url_dp2dul($variables['location']['title'], $request_actor);
    }
  }
  
  // organiser
  if (isset($vevent['organiser'])) {
    $organiser = $event['organiser'];
    
    // organiser title
    if (isset($organiser['actor']['detail']['nl']['title'])) {
      $variables['organiser']['title'] = $organiser['actor']['detail']['nl']['title'];
    }
    
    // organiser link
    if (isset($organiser['actor']['asset']) && $organiser['actor']['asset']) {
      $request_actor = array('actor' => $organiser['actor']['cdbid'], 'title' => $variables['organiser']['title']);
      $variables['organiser']['link'] = cnapi_url_dp2dul($variables['organiser']['title'], $request_actor);
    }
  }
  
  // contact
  if (isset($event['contactinfo']['main']) || isset($event['contactinfo']['other'])) {
    if (isset($event['contactinfo']['main'])) {
      $contact = $event['contactinfo']['main'];
    }
    elseif (isset($event['contactinfo']['other'])) {
      $contact = $event['contactinfo']['other'];
    }
    
    // mail
    if (isset($contact['mail'])) {
      $variables['contact']['mail'] = l($contact['mail'], 'mailto:' . $contact['mail']);
    }
    
    // phone
    if (isset($contact['phone'])) {
      $variables['contact']['phone'] = $contact['phone'];
    }
    
    // fax
    if (isset($contact['fax'])) {
      $variables['contact']['fax'] = $contact['fax'];
    }
  }
  
  // reservation
  if (isset($event['contactinfo']['reservation'])) {
    $reservation = $event['contactinfo']['reservation'];
    
    // mail
    if (isset($reservation['mail'])) {
      $variables['reservation']['mail'] = l($reservation['mail'], 'mailto:' . $reservation['mail']);
    }
    
    // phone
    if (isset($reservation['phone'])) {
      $variables['reservation']['phone'] = $reservation['phone'];
    }
    
    // fax
    if (isset($reservation['fax'])) {
      $variables['reservation']['fax'] = $reservation['fax'];
    }
  }
}