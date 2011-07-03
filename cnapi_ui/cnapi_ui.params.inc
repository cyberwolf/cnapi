<?php

function cnapi_ui_remove_context_query(&$query, $context) {
  $context = cnapi_ui_contexts('id', $context);
  if (isset($context['query'])) {
    foreach ($context['query'] as $key => $value) {
      if (isset($query[$key]) && $query[$key] == $value) {
        unset($query[$key]);
      }
    }
  }
}

function cnapi_ui_apply_context_query(&$query, $context) {
  $context = cnapi_ui_contexts('id', $context);

  if (isset($context['query'])) {
    foreach ($context['query'] as $key => $value) {
      $query[$key] = $value;
    }
  }
}

function cnapi_url_p2dul($text, $request, $context) {
  $request_dp = cnapi_url_p2dp($request, $context);

  return cnapi_url_dp2dul($text, $request_dp);
}

function cnapi_url_dp2dul($text, $request) {
  $url = cnapi_url_dp2dua($request);

  return l($text, $url['path'], $url['options']);
}

function cnapi_url_dp2dus($request) {
  $url = cnapi_url_dp2dua($request);
  return url($url['path'], $url['options']);
}

function cnapi_url_dp2dua($request) {
  $url = array('path' => '', 'options' => array());

  if (isset($request['context'])) {
    $context = cnapi_ui_contexts('id', $request['context']);

    if ($context) {
      $url['path'] = $context['path'];

      $query = isset($request['query']) ? $request['query'] : NULL;
      cnapi_ui_remove_context_query($query, $request['context']);

      if ($query) {
        $url['options']['query'] = $query;
      }
    }
  }
  else {
    if (isset($request['event'])) {
      $type = 'event';
    }
    elseif (isset($request['actor'])) {
      $type = 'actor';
    }
    elseif (isset($request['production'])) {
      $type = 'production';
    }

    $title = 'untitled';
    if (isset($request['title'])) {
      $title = $request['title'];
    }

    $cdbid = $request[$type];

    $url['path'] = 'agenda/' . $type[0] . '/' . cnapi_ui_slug($title) . '/' . $cdbid;
  }

  return $url;
}

function cnapi_url_dus2dp($url) {
  $url = ltrim($url, '/');
  extract(parse_url($url));

  $url = array('path' => $path);
  if ($query) {
    $url['options'] = array('query' => parse_str($query));
  }

  return cnapi_url_dua2dp($url);
}

function cnapi_url_dua2dp($url) {
  $request = FALSE;

  $path = $url['path'];

  if (arg(0, $path) == 'agenda' && in_array(arg(1), array('a', 'e', 'p'))) {
    $types = array(
      'a' => 'actor',
      'e' => 'event',
      'p' => 'production',
    );
    $type = $types[arg(1)];
    $request[$type] = arg(3);
  }

  else {
    if ($context = cnapi_ui_contexts('path', $path)) {
      $request['context'] = $context['id'];
      $request['query'] = $url['options']['query'];
    }
  }

  // @todo filter unwanted query strings (vb. utm_source, ...)

  return $request;
}

function cnapi_url_dp2p($request) {
  $request_p = FALSE;

  if (isset($request['context'])) {
    $request_p['action'] = CNAPI_LIST_SUMMARY;

    $context = cnapi_ui_contexts('id', $request['context']);

    $request_p['type'] = $context['type'];
    $request_p['query'] = $request['query'];

    if ($query = $context['query']) {
      foreach ($query as $key => $value) {
        $request_p['query'][$key] = $value;
      }
    }

    if (isset($request_p['query']['page'])) {
      $request_p['query']['page']++;
    }

    if (isset($request_p['query']['query'])) {
      $request_p['query']['q'] = $request_p['query']['query'];
      unset($request_p['query']['query']);
    }
  }
  else {
    $request_p['action'] = 'detail';

    if (isset($request['event'])) {
      $type = 'event';
    }
    elseif (isset($request['actor'])) {
      $type = 'actor';
    }
    elseif (isset($request['production'])) {
      $type = 'production';
    }

    $request_p = array(
      'action' => 'detail',
      'type' => $type,
      'cdbid' => $request[$type],
    );
  }

  return $request_p;
}

function cnapi_url_dp2a($request) {
  $request = cnapi_url_dp2p($request);
  return cnapi_url_p2a($request);
}

function cnapi_url_dus2p($url) {
  $request = cnapi_url_dus2dp($url);
  return cnapi_url_dp2p($request);
}

function cnapi_url_dua2p($url) {
  $request = cnapi_url_dua2dp($url);
  return cnapi_url_dp2p($request);
}

function cnapi_url_dus2a($url) {
  $request = cnapi_url_dus2p($url);
  return cnapi_url_p2a($url);
}

function cnapi_url_dua2a($url) {
  $request = cnapi_url_dua2p($url);
  return cnapi_url_p2a($url);
}

function cnapi_url_p2dua($request, $context) {
  $request = cnapi_url_p2dp($request, $context);
  return cnapi_url_dp2dua($request);
}

function cnapi_url_p2dus($request, $context) {
  $request = cnapi_url_p2dp($request, $context);
  return cnapi_url_dp2dus($request);
}

function cnapi_url_p2dp($request, $context = FALSE) {
  $request_dp = FALSE;

  if ($request['action'] == 'detail') {
    $type = $request['type'];
    $request_dp[$type] = $request['query']['cdbid'];
  }
  elseif ($request['action'] == 'list_summary') {
    if ($context === TRUE) {
      // @todo try to detect context
    }
    elseif ($context) {
    }
    else {
      $context = cnapi_ui_get_active_context(); // if there's no active context, default context should be determined
    }

    $request_dp['context'] = $context;
    $request_dp['query'] = $request['query'];

    cnapi_ui_remove_context_query($request_dp['query'], $context);
  }

  return $request_dp;
}

function cnapi_url_a2dua($request, $context) {
  $request = cnapi_url_a2p($request);
  return cnapi_url_p2dua($request, $context);
}

function cnapi_url_a2dus($request, $context) {
  $request = cnapi_url_a2p($request);
  return cnapi_url_p2dus($request, $context);
}

function cnapi_url_a2dp($request, $context) {
  $request = cnapi_url_a2p($request);
  return cnapi_url_p2dp($request, $context);
}