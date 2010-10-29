<?php
// $Id$

function cnapi_ui_url_du2aa($url = '') {
  $request = array();

  // if a url starts with a slash, remove it
  $url = ltrim($url, '/');

  // extract $path and $query
  extract(parse_url($url));

  // check if we have a valid request
  if (!(arg(0, $path) == 'cn' && in_array(arg(1, $path), array('list', 'detail')))) {
    return FALSE;
  }

  // get action and validate it
  switch (arg(1, $path)) {
    case 'detail':
      $request['action'] = 'detail';
      break;
    case 'list':
      $request['action'] = 'list_summary';
      break;
    default:
      return FALSE;
  }

  // get type and validate it
  $request['type'] = arg(2, $path);
  if (!in_array($request['type'], array('event', 'production', 'actor'))) {
    return FALSE;
  }

  // get cdbid in case of detail
  if ($request['action'] == 'detail') {
    $request['query']['cdbid'] = arg(4, $path);
  }

  // get query in case of list_summary
  if ($request['action'] == 'list_summary' && isset($query)) {
    parse_str($query, $request['query']);
  }

  // quirks
  if (isset($request['query'])) {
    if (isset($request['query']['query'])) {
      $request['query']['q'] = $request['query']['query'];
      unset($request['query']['query']);
    }
    if (isset($request['query']['page']) && is_numeric($request['query']['page'])) {
      $request['query']['page'] = $request['query']['page'] + 1;
    }
  }

  return $request;
}

function cnapi_ui_url_aa2da($request = array()) {
  cnapi_request_remove_defaults($request);
  
  if (!in_array($request['type'], array('event', 'production', 'actor'))) {
    return FALSE;
  }
    
  $slug = 'undefined';
  if (isset($request['query']['title'])) $slug = cnapi_ui_slug($request['query']['title'], variable_get('cnapi_max_slug_length', -1));

  if ($request['action'] == 'detail') {
    switch ($request['type']) {
      case 'event':
        $path = sprintf('cn/detail/event/%s/%s', $slug, $request['query']['cdbid']);
        $query = array();
        break;
      case 'actor':
        $path = sprintf('cn/detail/actor/%s/%s', $slug, $request['query']['cdbid']);
        $query = array();
        break;
      case 'production':
        $path = sprintf('cn/detail/production/%s/%s', $slug, $request['query']['cdbid']);
        $query = array();
        break;
    }
  }
  elseif ($request['action'] == 'list_summary') {
    $path = sprintf('cn/list/%s', $request['type']);
    $query = $request['query'];
  }

  // quirks
  if (isset($query)) {
    if (isset($query['q'])) {
      $query['query'] = $request['query']['q'];
      unset($query['q']);
    }
    if (isset($query['page']) && is_numeric($query['page'])) {
      $query['page'] = $query['page'] - 1;
    }
  }

  return array('path' => $path, 'query' => isset($query) ? $query : array());
}

function cnapi_ui_url_aa2url($request, $options = array()) {
  $url = cnapi_ui_url_aa2da($request);
  if (isset($url['query'])) {
    if (!isset($options['query'])) {
      $options['query'] = array();
    }
    $options['query'] = array_merge($url['query'], $options['query']);
  }
  return url($url['path'], $options);
}

function cnapi_ui_url_aa2l($text, $request, $options = array()) {
  $url = cnapi_ui_url_aa2da($request);
  if (isset($url['query'])) {
    if (!isset($options['query'])) {
      $options['query'] = array();
    }
    $options['query'] = array_merge($url['query'], $options['query']);
  }
  return l($text, $url['path'], $options);
}

function cnapi_ui_url_du2au($url = '') {
  $url = ltrim($url, '/');
  $request = cnapi_ui_url_du2aa($url);
  return cnapi_ui_url_p2a($request);
}

function cnapi_ui_url_au2da($url = '', $title = '') {
  $url = ltrim($url, '/');
  $request = cnapi_ui_url_a2p($url);
  if ($title) {
    $request['query']['title'] = $title;
  }
  return cnapi_ui_url_aa2da($request, $full, $absolute);
}

function cnapi_ui_url_au2l($text, $url = '', $title = '', $options = array()) {
  $url = ltrim($url, '/');
  $request = cnapi_ui_url_a2p($url);
  if ($title) {
    $request['query']['title'] = $title;
  }
  return cnapi_ui_url_aa2l($text, $request, $options);
}

function cnapi_ui_url_a2url($url = '', $title = '', $options = array()) {
  $url = ltrim($url, '/');
  $request = cnapi_ui_url_a2p($url);
  if ($title) {
    $request['query']['title'] = $title;
  }
  return cnapi_ui_url_aa2url($request, $options);
}

function cnapi_ui_get_request() {
  $request = array();
  $qs_request = $_GET;
  unset($qs_request['q']);
  $url = $_GET['q'];
  $qs = http_build_query($qs_request);
  if (strlen($qs) > 0) $url .= '?' . $qs;
  $request = cnapi_ui_url_du2aa($url);

  cnapi_request_add_defaults($request);
  cnapi_request_clean($request);

  return $request;
}

function cnapi_ui_goto($request) {
  // construct url
  $url = cnapi_ui_url_da2url($request);
  $url = ltrim($url, '/');
  extract(parse_url($url));
  $path = substr($path, strlen(base_path()) - 1, strlen($path) - strlen(base_path()) + 1);

  // redirect
  drupal_goto($path, array('query' => $query));
}