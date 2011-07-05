<?php

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event_summary(&$variables) {
  // cdbid
  $variables['cdbid'] = $variables['event']['cdbid'];

  // title
  $variables['title'] = check_plain($variables['event']['title']);

  // object_url
  $request_detail = array(
    'event' => $variables['event']['cdbid'],
    'title' => $variables['event']['title'],
  );

  $variables['object_url'] = cnapi_url_dp2dus($request_detail);

  // thumbnail
  $variables['thumbnail'] = isset($variables['event']['thumbnail']) && !empty($variables['event']['thumbnail']) ? $variables['event']['thumbnail'] : url(drupal_get_path('module', 'cnapi_ui') . '/img/no-image.jpg');

  // shortdescription
  $variables['shortdescription'] = isset($variables['event']['shortdescription']) && !empty($variables['event']['shortdescription']) ? truncate_utf8(strip_tags($variables['event']['shortdescription']), 200, TRUE, TRUE) : NULL;

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