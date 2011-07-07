<?php

/**
 * Preprocess variables for cnapi-extras-block-wider-radius-search.tpl.php.
 *
 * @see cnapi-extras-block-wider-radius-search.tpl.php
 */
function template_preprocess_cnapi_extras_block_wider_radius_search(&$variables) {
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
  
  $variables['info'] = t('!total results found !radius km around !city.', $replace);
  $variables['link'] = l(t('View all results'), $url['path'], $url['options']);
}