<?php

/**
 * Preprocess variables for cnapi-extras-block-wider-radius-search.tpl.php.
 *
 * @see cnapi-extras-block-wider-radius-search.tpl.php
 */
function template_preprocess_cnapi_extras_block_wider_radius_search(&$variables) {
  $type = $variables['request_type'];

  $city = $variables['city'];
  if ($variables['city_type'] == 'cityid') {
    $cities = cnapi_get_cities();
    $city = $cities[$city]['name'];
  }

  $url = cnapi_url_dp2dua($variables['request']);

  $replace = array(
    '!total' => $variables['total'],
    '!radius' => $variables['radius'],
    '!city' => $city,
  );

  if ($variables['radius'] == -1) {
    $variables['info'] = t('!total results found in Flanders.', $replace);
  }
  else {
    $variables['info'] = t('!total results found !radius km around !city.', $replace);
  }
  $variables['link'] = l(t('View all results'), $url['path'], $url['options']);

  $data = array_slice($variables['data'], 0, 3);

  $items = array();
  foreach ($data as $object) {
    $items[] = theme('cnapi_ui_' . $type . '_summary', array($type => $object));
  }

  $variables['objects'] = theme('item_list', array('items' => $items));
}