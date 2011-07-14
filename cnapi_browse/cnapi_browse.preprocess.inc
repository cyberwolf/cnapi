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
  $variables['id'] = $variables['item']['id'];
  $variables['name'] = $variables['item']['name'];
  if (isset($variables['item']['total'])) {
    $variables['total'] = $variables['item']['total'];
  }
  $variables['link'] = cnapi_url_dp2dul($variables['name'], $variables['item']['url']);
  $variables['is_heading_group'] = $variables['item']['is_heading_group'];
}

/**
 * Preprocess variables for cnapi-browse-filter-option.tpl.php.
 *
 * @see cnapi-browse-filter-option.tpl.php
 */
function template_preprocess_cnapi_browse_filter_option(&$variables) {
  $variables['id'] = $variables['option']['id'];
  $variables['name'] = $variables['option']['name'];
  $variables['active'] = $variables['option']['active'];
  if (isset($variables['option']['total'])) {
    $variables['total'] = $variables['option']['total'];
  }
  $variables['link'] = cnapi_url_dp2dul($variables['name'], $variables['option']['url']);
  if (isset($variables['option']['url_remove'])) {
    $variables['link_remove'] = cnapi_url_dp2dul('x', $variables['option']['url_remove']);
  }
}