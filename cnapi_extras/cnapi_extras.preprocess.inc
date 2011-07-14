<?php

/**
 * @file
 * Preprocessing.
 */

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
    $variables['info'] = t('!total results found in Flanders.', $replace);
  }
  
  // ... else we had a hit for a certain city and radius.
  else {
    $variables['info'] = t('!total results found !radius km around !city.', $replace);
  }
  
  // Construct a "view all results" link.
  $variables['link'] = l(t('View all results'), $url['path'], $url['options']);

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