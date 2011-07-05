<?php

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
  $request_detail = array(
    $type => $object['cdbid'],
    'title' => $object['title'],
  );

  $variables['object_url'] = cnapi_url_dp2dus($request_detail);

  // thumbnail
  // @todo provide -no-image
  $variables['thumbnail'] = isset($object['thumbnail']) && !empty($object['thumbnail']) ? $object['thumbnail'] : url(drupal_get_path('module', 'cnapi_ui') . '/img/no-image.jpg');

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

  // vlieg
  $variables['is_vlieg'] = isset($event['agefrom']) && $event['agefrom'] < 12;
  // @todo $variables['vlieg_image']
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
 *
 * @see cnapi-ui-production.tpl.php
 */
function template_preprocess_cnapi_ui_production_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'production');

  $production = $variables['production'];

  // performers
  _template_preprocess_cnapi_ui_object_performers($variables, 'event');

  // vlieg
  $variables['is_vlieg'] = isset($production['agefrom']) && $production['agefrom'] < 12;
  // @todo $variables['vlieg_image']
}