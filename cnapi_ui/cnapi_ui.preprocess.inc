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
  $variables['is_vlieg'] = FALSE;
  
  // is_vlieg
  if (isset($object['agefrom'])) {
    $variables['is_vlieg'] = $object['agefrom'] < 12;;
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
  $variables['shortdescription'] = isset($detail['shortdescription']) && !empty($detail['shortdescription']) ? strip_tags($detail['shortdescription']) : NULL;

  // longdescription
  $variables['longdescription'] = isset($detail['longdescription']) && !empty($detail['longdescription']) ? $detail['longdescription'] : NULL;
  
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
  if (isset($detail['price']['value'])) {
    if ($detail['price']['value'] == '0') {
      $variables['price'] = 'Gratis';
    }
    else {
      $variables['price'] = '&euro; '. $detail['price']['value'];
    }

    if ($detail['price']['description']) {
      $variables['price'] .= ' ('. $detail['price']['description'] .')';
    }
  }
  elseif (isset($detail['price']['description'])) {
    $variables['price'] = $detail['price']['description'];
  }

  // vlieg
  _template_preprocess_cnapi_ui_object_vlieg($variables, 'event');
}

// also look at https://cultuurnet.beanstalkapp.com/m-uitinvlaanderen-be/browse/trunk/sites/all/modules/custom/cnapi_mobile/cnapi_mobile.module
// also look at https://cultuurnet.beanstalkapp.com/m-uitinvlaanderen-be/browse/trunk/sites/all/modules/custom/cnapi_mobile/cnapi-mobile-event.tpl.php

/*
function cnapi_ui_preprocess_cnapi_event(&$vars) {

 
  // performers
  $performers = array();
  if ($vars['event']['performers']) {
    foreach ($vars['event']['performers'] as $performer) {
      $performers[] = $performer['performer_title'];
    }
  }
  $vars['event']['performers_flat'] = implode(', ', $performers);

  // address
  _cnapi_ui_merge_address($vars['event'], 'where');

  // where
  $where = $vars['event']['where_title'];
  if ($vars['event']['where_id']  && !$vars['event']['where_dummy'] && $vars['event']['where_asset'] == TRUE) {
    $where = cnapi_url_p2l($vars['event']['where_title'], array('action' => 'actor', 'query' => array('cdbid' => $vars['event']['where_id'], 'title' => $vars['event']['where_title'])), array('attributes' => array('class' => 'tooltip-enabled')));
  }
  $vars['event']['where'] = $where;

  // organiser
  if ($vars['event']['organiser_title'] != $vars['event']['where_title']) {
    $organiser = $vars['event']['organiser_title'];
    if ($vars['event']['organiser_id'] && !$vars['event']['organiser_dummy'] && $vars['event']['organiser_asset'] == TRUE) {
      $organiser = cnapi_url_p2l($vars['event']['organiser_title'], array('action' => 'actor', 'query' => array('cdbid' => $vars['event']['organiser_id'], 'title' => $vars['event']['organiser_title'])));
    }
    $vars['event']['organiser'] = $organiser;
  }

  // categories
  _cnapi_ui_categories($vars['event']);

  // links
  $links = array();
  if ($vars['event']['contact_url']) {
    foreach ($vars['event']['contact_url'] as $url) {
      $links[] = $url;
    }
  }
  if ($vars['event']['where_url']) {
    foreach ($vars['event']['where_url'] as $url) {
      $links[] = $url;
    }
  }
  if ($vars['event']['organiser_url']) {
    foreach ($vars['event']['organiser_url'] as $url) {
      $links[] = $url;
    }
  }
  if ($vars['event']['performers']) {
    foreach ($vars['event']['performers'] as $performer) {
      if (isset($performer['performer_url'])) {
        foreach ($performer['performer_url'] as $url) {
          $links[] = $url;
        }
      }
    }
  }

  $vars['event']['links'] = array_unique($links);

  $links = array();
  foreach ($vars['event']['links'] as $link) {
    if (!isset($vars['event']['reservation_url']) || (isset($vars['event']['reservation_url']) && !in_array($link, $vars['event']['reservation_url']))) {
      $links[] = l($link, $link, array('attributes' => array('target' => '_blank')));
    }
  }
  $vars['event']['links'] = $links;

  // order link
  if (isset($vars['event']['order_link'])) {
	  $vars['event']['order_link'] = l('Bestel tickets', $vars['event']['order_link'], array('attributes' => array('target' => '_blank', 'class' => 'event-order-link button')));
  }
}

function _cnapi_ui_merge_address(&$object, $prefix = 'address') {
  $parts = array();
  if ($object[$prefix .'_street']) $parts[] = $object[$prefix .'_street'];
  if ($object[$prefix .'_housenr']) $parts[] = $object[$prefix .'_housenr'];
  $street_nr = implode(' ', $parts);

  $parts = array();
  if ($object[$prefix .'_zipcode']) $parts[] = $object[$prefix .'_zipcode'];
  if ($object[$prefix .'_city']) $parts[] = $object[$prefix .'_city'];
  $zip_city = implode(' ', $parts);

  $parts = array();
  if ($street_nr) $parts[] = $street_nr;
  if ($zip_city) $parts[] = $zip_city;
  if ($object[$prefix .'_country'] && $object[$prefix .'_country'] != 'BE') $parts[] = $object[$prefix .'_country'];
  $object['address'] = implode(', ', $parts);
}

function _cnapi_ui_categories(&$object) {
  // tags
  if (count($object['keywords']) > 0 ) {
    foreach ($object['keywords'] as $keyword) {
      $object['tags'][] = cnapi_url_p2l($keyword, array('action' => 'events/search', 'query' => array('k' => str_replace(" ","_",$keyword))));
    }
  }

  // facilities
  if (count($object['facilities']) > 0) {
    $facilities = array();
    foreach ($object['facilities'] as $id => $facility) {
      $facilities[] = cnapi_url_p2l($facility, array('action' => 'events/search', 'query' => array('facility' => $id)));
    }
    $object['facilities'] = $facilities;
  }

  // targetaudiences
  if (count($object['targetaudiences']) > 0) {
    $targetaudiences = array();
    foreach ($object['targetaudiences'] as $id => $audience) {
      $targetaudiences[] = cnapi_url_p2l($audience, array('action' => 'events/search', 'query' => array('targetaudience' => $id)));
    }
    $object['targetaudiences'] = $targetaudiences;
  }
}

*/