<?php

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event_summary(&$variables) {
  $variables['cdbid'] = $variables['event']['cdbid'];
  
  $variables['title'] = $variables['event']['title'];
  
  $request_detail = array(
    'event' => $variables['event']['cdbid'],
    'title' => $variables['event']['title'],
  );
  
  $variables['object_url'] = cnapi_url_dp2dus($request_detail);
}

// performer urls : in preprocessing generation, in processing truncating