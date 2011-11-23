<?php

/**
 * @file
 * Handles tracking on the Cultuurnet Tracking API for lists (contexts) and detail pages.
 */

/**
 * Helper to add tracking calls on the lists and detail pages. Should be called from a place like hook_init.
 */
function cnapi_ui_tracking() {
  // Get the current request.
  $request = cnapi_ui_get_active_request();

  // We only act upon cnapi requests.
  if (!$request) {
    return;
  }

  // Tracking lists (context pages).
  if (isset($request['context'])) {
    $context = cnapi_ui_contexts('id', $request['context']);

    // Activity : list
    $activity = 1;

    // Params : context (event, movie, ...), type (actor, event, production) and query (the filters).
    $params = array();
    if ($context) {
      $params = $request['query'];
      $params['context'] = $request['context'];
      $params['type'] = $context['type'];
    }
  }

  // Tracking detail pages.
  else {
    // Determining the type
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

    // Activity : detail
    $activity = 2;

    // Params : type (actor, event, production)
    if (isset($type)) {
      $params['type'] = $type;
      $params['cdbid'] = $request[$type];
    }
  }

  // If we could successfully determine some tracking parameters, invoke the actual tracking call.
  if (!empty($params)) {
    cnapi_tracking_track($activity, $params);
  }
}