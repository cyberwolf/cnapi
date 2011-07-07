<?php

function cnapi_ui_tracking() {
  $request = cnapi_ui_get_active_request();

  if ($request) {
    if (isset($request['context'])) {
      $context = cnapi_ui_contexts('id', $request['context']);

      $activity = 'list';
      $params = array();

      if ($context) {
        $params = $request['query'];
        $params['context'] = $request['context'];
        $params['type'] = $context['type'];
      }
    }
    else {
      $activity = 'detail';
      $params = array();

      if (isset($request['actor'])) {
        $type = 'actor';
      }
      elseif (isset($request['event'])) {
        $type = 'event';
      }
      elseif (isset($request['production'])) {
        $type = 'production';
      }

      if (isset($type)) {
        $params['type'] = $type;
        $params['cdbid'] = $request[$type];
      }
    }

    if (!empty($params)) {
      cnapi_tracking_track($activity, $params);
    }
  }
}