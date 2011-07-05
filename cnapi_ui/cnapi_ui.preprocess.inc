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
  $default_image = url(drupal_get_path('module', 'cnapi_ui') . '/img/default-image.jpg');
  $thumbnail_url = isset($object['thumbnail']) && !empty($object['thumbnail']) ? $object['thumbnail'] : $default_image;
  $thumbnail_img = theme('image', array('path' => $thumbnail_url, 'alt' => $variables['title']));
  $variables['thumbnail'] = l($thumbnail_img, $variables['object_url'], array('html' => TRUE));

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
  $vlieg_description = 'Hallo, ik ben Vlieg en ik wijs de weg naar leuke activiteiten voor kinderen. Meer info op www.vliegjemee.be!';
  
  $vlieg_image_url = url(drupal_get_path('module', 'cnapi_ui') . '/img/vlieg.jpg');
  $variables['vlieg_image'] = theme('image', array('path' => $vlieg_image_url, 'alt' => $vlieg_description, 'title' => $vlieg_description));
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

  // vlieg
  $variables['is_vlieg'] = isset($event['agefrom']) && $event['agefrom'] < 12;
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
  _template_preprocess_cnapi_ui_object_vlieg($variables, 'event');

  // vlieg
  $variables['is_vlieg'] = isset($production['agefrom']) && $production['agefrom'] < 12;
  
  // vlieg image
  $variables['vlieg_image'] = url(drupal_get_path('module', 'cnapi_ui') . '/img/vlieg.jpg');
}