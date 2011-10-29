<?php

/**
 * @file
 * Preprocessing.
 */

/**
 * Preprocess variables for cnapi-extras-actor-playlist-event.tpl.php.
 *
 * @see cnapi-extras-actor-playlist-event.tpl.php
 */
function template_preprocess_cnapi_extras_actor_playlist_event(&$variables) {
  $event = $variables['event'];
  
  // event_url
  $request_detail = array('event' => $event['cdbid'], 'title' => $event['title']);
  $event_url = cnapi_url_dp2dua($request_detail);

  // more_link
  $variables['more_link'] = l('Meer info', $event_url['path'], $event_url['options']);
  
  // title
  $variables['title'] = l($event['title'], $event_url['path'], $event_url['options']);
  
  // calendarsummary
  $variables['calendarsummary'] = $event['calendarsummary'];
  
  // thumbnail
  $thumbnail_url = isset($event['thumbnail']) && !empty($event['thumbnail']) ? $event['thumbnail'] : $default_image;
  $thumbnail_img = theme('image', array('path' => $thumbnail_url, 'alt' => $variables['title']));
  $event_url['options']['html'] = TRUE;
  $variables['thumbnail'] = l($thumbnail_img, $event_url['path'], $event_url['options']);
}

/**
 * Preprocess variables for cnapi-extras-block-movie-results.tpl.php.
 *
 * @see cnapi-extras-block-movie-results.tpl.php
 */
function template_preprocess_cnapi_extras_block_movie_results(&$variables) {
  $items = array();
  foreach ($variables['productions'] as $production) {
    $items[] = theme('cnapi_extras_movie_summary', array('production' => $production));
  }
  $variables['productions'] = theme('item_list', array('items' => $items));
  
  // more link
  if ($variables['total'] > count($variables['productions'])) {
    $text = sprintf('Toon alle %d films voor deze zoekopdracht', $variables['total']);
    $variables['more_link'] = cnapi_url_dp2dul($text, $variables['more_request']);
  }
}

/**
 * Preprocess variables for cnapi-extras-movie-summary.tpl.php.
 *
 * @see cnapi-extras-movie-summary.tpl.php
 */
function template_preprocess_cnapi_extras_movie_summary(&$variables) {
  $object = $variables['production'];
  
  // object_url
  $request_detail = array('production' => $object['cdbid'], 'title' => $object['title']);
  $object_url = cnapi_url_dp2dua($request_detail);

  // more_link
  $variables['more_link'] = l('Meer info', $object_url['path'], $object_url['options']);
  
  // title
  $variables['title'] = l($object['title'], $object_url['path'], $object_url['options']);
  
  // thumbnail
  $thumbnail_url = isset($object['thumbnail']) && !empty($object['thumbnail']) ? $object['thumbnail'] : $default_image;
  $thumbnail_img = theme('image', array('path' => $thumbnail_url, 'alt' => $variables['title']));
  $object_url['options']['html'] = TRUE;
  $variables['thumbnail'] = l($thumbnail_img, $object_url['path'], $object_url['options']);
}

/**
 * Preprocess variables for cnapi-extras-block-wider-radius-search.tpl.php.
 *
 * @see cnapi-extras-block-wider-radius-search.tpl.php
 */
function template_preprocess_cnapi_extras_block_wider_radius_search(&$variables) {
  $type = $variables['request_type'];

  // In case the city was specified using its id, we still need to find the name.
  $city = $variables['city'];
  if ($variables['city_type'] == 'cityid') {
    $cities = cnapi_get_cities();
    $city = $cities[$city]['name'];
  }

  // Convert the Drupal request array to a Drupal url array with path and options part.
  $url = cnapi_url_dp2dua($variables['request']);

  // Gather the replacement variables for our translation (t) string.
  $replace = array(
    '!total' => $variables['total'],
    '!radius' => $variables['radius'],
    '!city' => $city,
  );

  // In case the radius was -1, we had a hit for the whole of Flanders...
  if ($variables['radius'] == -1) {
    $variables['info'] = strstr('!total resultaten gevonden in Flanders.', $replace);
  }
  
  // ... else we had a hit for a certain city and radius.
  else {
    $variables['info'] = strstr('!total resultaten gevonden !radius km around !city.', $replace);
  }
  
  // Construct a "view all results" link.
  $variables['link'] = l('Bekijk alle resultaten', $url['path'], $url['options']);

  // Get the top 3 result...
  $data = array_slice($variables['data'], 0, 3);
  
  // ... and construct some themed html result from them ...
  $items = array();
  foreach ($data as $object) {
    $items[] = theme('cnapi_ui_' . $type . '_summary', array($type => $object));
  }

  // ... and save it in the objects variable.
  $variables['objects'] = theme('item_list', array('items' => $items));
}