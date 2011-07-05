<?php

/**
 * Preprocess variables for cnapi-ui-actor.tpl.php.
 *
 * @see cnapi-ui-actor.tpl.php
 */
function _template_preprocess_cnapi_ui_object_summary(&$variables, $type) {
  // cdbid
  $variables['cdbid'] = $variables[$type]['cdbid'];

  // title
  $variables['title'] = check_plain($variables[$type]['title']);

  // object_url
  $request_detail = array(
    $type => $variables[$type]['cdbid'],
    'title' => $variables[$type]['title'],
  );

  $variables['object_url'] = cnapi_url_dp2dus($request_detail);

  // thumbnail
  // @todo provide -no-image
  $variables['thumbnail'] = isset($variables[$type]['thumbnail']) && !empty($variables[$type]['thumbnail']) ? $variables[$type]['thumbnail'] : url(drupal_get_path('module', 'cnapi_ui') . '/img/no-image.jpg');

  // shortdescription
  $variables['shortdescription'] = isset($variables[$type]['shortdescription']) && !empty($variables[$type]['shortdescription']) ? truncate_utf8(strip_tags($variables[$type]['shortdescription']), 200, TRUE, TRUE) : NULL;
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

  // performers
  $variables['performers'] = NULL;

  if (isset($variables['event']['performers'])) {
    $performers = array_slice($variables['event']['performers'], 0, 3);
    $variables['performers'] = implode(', ', $performers);

    if (count($variables['event']['performers']) > 3) {
      $variables['performers'] .= ',&nbsp;&hellip;';
    }
  }

  // where
  $where = array();
  if (isset($variables['event']['location']) && !empty($variables['event']['location'])) {
    $where[] = check_plain($variables['event']['location']);
  }
  if (isset($variables['event']['city']) && !empty($variables['event']['city'])) {
    $where[] = check_plain($variables['event']['city']);
  }
  $variables['where'] = !empty($where) ? implode(', ', $where) : NULL;

  // when
  $variables['when'] = isset($variables['event']['calendarsummary']) ? $variables['event']['calendarsummary'] : NULL;
  
  // vlieg
  $variables['is_vlieg'] = isset($variables['event']['agefrom']) && $variables['event']['agefrom'] < 12;
  // @todo $variables['vlieg_image']
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
 *
 * @see cnapi-ui-production.tpl.php
 */
function template_preprocess_cnapi_ui_production_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'production');

  // performers
  $variables['performers'] = NULL;

  if (isset($variables['production']['performers'])) {
    $performers = array_slice($variables['production']['performers'], 0, 3);
    $variables['performers'] = implode(', ', $performers);

    if (count($variables['production']['performers']) > 3) {
      $variables['performers'] .= ',&nbsp;&hellip;';
    }
  }
  
  // vlieg
  $variables['is_vlieg'] = isset($variables['production']['agefrom']) && $variables['production']['agefrom'] < 12;
  // @todo $variables['vlieg_image']
}