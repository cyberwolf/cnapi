<?php

/**
 * @file
 * Preprocessing.
 */
 
/**
 * Preprocess variables for cnapi-browse-navigation-item.tpl.php.
 *
 * @see cnapi-browse-navigation-item.tpl.php
 */
function template_preprocess_cnapi_browse_navigation_item(&$variables) {
  // id
  $variables['id'] = $variables['item']['id'];
  
  // name
  $variables['name'] = $variables['item']['name'];
  
  // total
  if (isset($variables['item']['total'])) {
    $variables['total'] = $variables['item']['total'];
  }
  
  // link
  $text = $variables['name'];
  if (!$variables['item']['is_heading_group']) {
    $text .= ' <span class="total">(' . $variables['total'] . ')</span>';
  }
  
  $url = cnapi_url_dp2dua($variables['item']['url']);
  $url['options']['html'] = TRUE;
  $variables['link'] = l($text, $url['path'], $url['options']);
  
  // is_heading_group
  $variables['is_heading_group'] = $variables['item']['is_heading_group'];
}

/**
 * Preprocess variables for cnapi-browse-filter-option.tpl.php.
 *
 * @see cnapi-browse-filter-option.tpl.php
 */
function template_preprocess_cnapi_browse_filter_option(&$variables) {
  // id
  $variables['id'] = $variables['option']['id'];
  
  // name
  $variables['name'] = $variables['option']['name'];
  
  // active
  $variables['active'] = $variables['option']['active'];
  
  // total
  if (isset($variables['option']['total'])) {
    $variables['total'] = $variables['option']['total'];
  }
  
  // link
  $variables['link'] = cnapi_url_dp2dul($variables['name'], $variables['option']['url']);
  
  // link_remove
  if (isset($variables['option']['url_remove'])) {
    $variables['link_remove'] = cnapi_url_dp2dul('x', $variables['option']['url_remove']);
  }
}