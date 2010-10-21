<?php
// $Id$

function cnapi_get($params) {
  cnapi_params_add_defaults($params);
  cnapi_params_clean($params);

  return _cnapi_get($params);
}

function cnapi_get_raw($params) {
  return _cnapi_get($params);
}

function cnapi_get_event($cdbid, $related_events = CNAPI_PARSE_IGNORE) {
  $params = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_PARSE_IGNORE) {
    $params['query']['related_events'] = $related_events;
  }
  return cnapi_get($params);
}

function cnapi_get_production($cdbid, $related_events = CNAPI_PARSE_IGNORE) {
  $params = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_PARSE_IGNORE) {
    $params['query']['related_events'] = $related_events;
  }
  return cnapi_get($params);
}

function cnapi_get_actor($cdbid) {
  $params = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $cdbid));
  return cnapi_get($params);
}

function cnapi_get_events($query = NULL, $format = CNAPI_PARSE_LIST) {
  $params = array('type' => 'event', 'query' => $query);
  $params['action'] = CNAPI_PARSE_DETAIL ? 'list' : 'summary';
  return cnapi_get($params);
}

function cnapi_get_productions($query = NULL, $format = CNAPI_PARSE_LIST) {
  $params = array('type' => 'production', 'query' => $query);
  $params['action'] = CNAPI_PARSE_DETAIL ? 'list' : 'summary';
  return cnapi_get($params);
}

function cnapi_get_actors($query = NULL, $format = CNAPI_PARSE_LIST) {
  $params = array('type' => 'actor', 'query' => $query);
  $params['action'] = CNAPI_PARSE_DETAIL ? 'list' : 'summary';
  return cnapi_get($params);
}

function cnapi_get_report($type, $query = NULL) {
  $params = array('action' => 'report', 'type' => $type, 'query' => $query);
  return cnapi_get($params);
}

function _cnapi_get($params = NULL) {
  if (!cnapi_params_validate($params)) {
    watchdog('cnapi', 'Invalid API request !params.', array('!params' => serialize($params)), WATCHDOG_ERROR);
    return FALSE;
  }

  $result = NULL;

  $cid = cnapi_cache_cid($params);

  $cache = _cnapi_cache_get($params);

  if ($cache != NULL) {
    $result = $cache;
  }
  else {
    $result = cnapi_api_request($params);

    if ($result) {
      $result = _cnapi_parse($params, $result);

      if (in_array($params['action'], array('list', 'summary'))) {
        $total = intval($result->nofrecords);
        _cnapi_cache_list_clean_by_total($params, $total);
      }

      _cnapi_cache_set($params, $result);
    }
  }

  return $result;
}

function _cnapi_parse($params, $result) {
  // @todo implement this
  return new SimpleXMLElement($result);
}

function _cnapi_cache_list_clean_by_total($params, $total) {
  // setting the cache and purgin invalid items
  // goal is to get consistent paging
  // algorithm: save total for a result set to cache. if another page from the same set is requested an it has a different total, clear all pages from result set.
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid_base = sprintf('%s:%s', $params['type'], cnapi_base_query_hash($params));
    $cid_total = sprintf('%s:total', $cid_base);
    if ($cache_total = cache_get($cid_total)) { // what if number of results = 0?
      if ($cache_total != $total) {
        cache_clear_all($cid_base, _cnapi_cache_table($params), TRUE);
        cache_set($cid_total, $total, _cnapi_cache_table($params));
      }
    }
    else {
      cache_set($cid_total, $total, _cnapi_cache_table($params));
    }
  }
}

function _cnapi_cache_table($params) {
  switch ($params['action']) {
    case 'detail':
      return 'cache_cnapi_detail';
    default:
      return 'cache_cnapi';
  }
}

function _cnapi_cache_expires($params) {
  $var = 'cnapi_cache_lifetime';

  if ($params['action'] == 'detail') {
    $var = 'cnapi_cache_lifetime_detail';
  }

  $expire = CACHE_TEMPORARY;
  if (variable_get($var, 0) > 0) {
    $expire = $_SERVER['REQUEST_TIME'] + variable_get($var, 0);
  }

  // @improvement add hook so other modules can alter this

  return $expire;
}

function cnapi_cache_cid($params) {
  unset($params['query']['format']);

  _cnapi_params_expand_key($params);
  unset($params['query']['key']);

  switch ($params['action']) {
    case 'detail':
      return implode(':', array($params['type'], cnapi_query_hash($params), $param['query']['cdbid']));
    case 'report':
      return implode(':', array($params['type'], cnapi_base_query_hash($params), $params['action']));
    default:
      return implode(':', array($params['type'], cnapi_base_query_hash($params), $params['action'], $params['query']['pagelength'], $params['query']['page'], $params['query']['sort']));
  }
}

function _cnapi_cache_get($params) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($params);
    $table = _cnapi_cache_table($params);
    if ($cache = cache_get($cid, $table)) {
      return $cache->data;
    }
  }
  return NULL;
}

function _cnapi_cache_set($params, $object) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($params);
    $table = _cnapi_cache_table($params);
    $expires = _cnapi_cache_expires($params);
    cache_set($cid, $object, $table, $expires);
  }
}

function cnapi_api_request($params) {
  if (!isset($params['query']['format'])) {
    $params['query']['format'] = 'xml';
  }
  $url = cnapi_url_p2a($params);
  $url = rtrim(variable_get('cnapi_api_location', CNAPI_API_LOCATION), '/') . '/api/' . $url;
  return cnapi_http_request($url);
}

function cnapi_http_request($url) {
  $reponse = FALSE;

  _cnapi_timer('start', $url);

  $ch = curl_init($url);

  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_TIMEOUT, CNAPI_HTTP_REQUEST_TIMEOUT);
  curl_setopt($ch, CURLOPT_ENCODING, 'gzip, deflate');

  if (variable_get('cnapi_proxy_enabled', FALSE)) {
    curl_setopt($ch, CURLOPT_PROXY, trim(variable_get('cnapi_proxy_server', '')));
    curl_setopt($ch, CURLOPT_PROXYPORT, variable_get('cnapi_proxy_port', ''));
    curl_setopt($ch, CURLOPT_PROXYUSERPWD, sprintf('%s:%s', variable_get('cnapi_proxy_username', ''), variable_get('cnapi_proxy_password', '')));
  }

  $response = curl_exec($ch);
  $curl_info = curl_getinfo($ch);

  curl_close($ch);

  _cnapi_timer('stop', $url);

  if ($curl_info['http_code'] != 200) return FALSE;

  if ($response) {
    return $response;
  }
  else {
    watchdog('cnapi', 'Error doing request !request in cnapi_request_api_xml : Return code was !error', array('!request' => $url, '!error' => $curl_info['http_code']), WATCHDOG_ERROR);
    return FALSE;
  }
}

function _cnapi_timer($action, $url = NULL) {
  $_cnapi_timers = &drupal_static(__FUNCTION__, array());

  $timer_key = 'cnapi_http_request_' . $url;
  if ($action == 'start') {
    timer_start($timer_key);
  }
  if ($action == 'stop') {
    $time = timer_stop($timer_key);
    $_cnapi_timers[$url] = $time['time'];
  }
  if ($action == 'read' && $url) {
    return $_cnapi_timers[$url];
  }
  if ($action == 'read' && $url == NULL) {
    return $_cnapi_timers;
  }
}

function cnapi_default_values($key = NULL, $namespace = NULL) {
  switch ($namespace) {
    case 'event':
      $defaults = variable_get('cnapi_defaults_events', array('pagelength' => -1, 'sort' => -1));
      break;
    case 'production':
      $defaults = variable_get('cnapi_defaults_productions', array('pagelength' => -1, 'sort' => -1));
      break;
    case 'actor':
      $defaults = variable_get('cnapi_defaults_actors', array('pagelength' => -1, 'sort' => -1));
      break;
    default:
      $defaults = variable_get('cnapi_defaults', array('pagelength' => CNAPI_DEFAULT_PAGELENGTH, 'sort' => CNAPI_DEFAULT_SORT));
      break;
  }

  if (!$key) {
    return $defaults;
  }
  else {
    return $defaults[$key];
  }
}

// remove_defaults should be part of ui module, we only deal with api stuff here
// stuff like pagelength, page and sort are always specified although not necessary to get a meaningful cache id (cid still works if defaults change)
function cnapi_params_add_defaults(&$params) {
  // api key
  if (!isset($params['query']['key'])) {
    $params['query']['key'] = variable_get('cnapi_api_key', '');
  }

  if (in_array($params['action'], array('list', 'summary'))) {
    // page
    if (!isset($params['query']['page'])) {
      $params['query']['page'] = 1;
    }

    // calculating defaults
    $defaults = cnapi_default_values();

    foreach (array('event', 'production', 'actor') as $type) {
      if ($params['type'] == $type) {
        $namespace_defaults = array();
        foreach (cnapi_default_values(NULL, $type) as $key => $val) {
          if ($val != -1) {
            $namespace_defaults[$key] = $val;
          }
        }
        $defaults = array_merge($defaults, $namespace_defaults);
      }
    }

    // setting defaults
    foreach ($defaults as $key => $value) {
      if (!isset($params['query'][$key])) {
        $params['query'][$key] = $value;
      }
    }
  }

  drupal_alter('cnapi_params_defaults', $params);

  return $params;
}

// cnapi_params_clean is very important in case of report because cache id depends on it, otherwise we get a different cache per page
// Does a best effort. : cleaning up for report, detail, ... but for "query" we don't care
function cnapi_params_clean(&$params) {
  $valid_query_detail = array('key', 'cdbid', 'relatedevents');
  $invalid_query_report = array('page', 'pagelength', 'sort');

  // clean detail requests
  if ($params['action'] == 'detail') {
    $params['query'] = array_intersect_key($params['query'], array_flip($valid_query_detail));
  }

  // clean report requests
  elseif ($params['action'] == 'report') {
    $params['query'] = array_diff_key($params['query'], array_flip($invalid_query_report));
  }
}

function cnapi_params_validate($params) {
  $valid_actions = array('list', 'summary', 'detail', 'report');
  $valid_types = array('event', 'production', 'actor');

  // all requests should have a key
  if (!$params['query']['key']) {
    return FALSE;
  }

  // check if action is valid
  if (!in_array($params['action'], $valid_actions)) {
    return FALSE;
  }

  // check if type is valid
  if (!in_array($params['type'], $valid_types)) {
    return FALSE;
  }

  // reports not valid on actors
  if ($params['type'] == 'actor' && $params['action'] == 'report') {
    return FALSE;
  }

  // validate $query keys for list, summary and report
  $valid_queries_common = array('key', 'q', 'k', 'changedsince', 'zip', 'city', 'cityid', 'regio', 'latlng', 'format', 'sort', 'page', 'pagelength');
  $valid_queries_events_productions = array('locationkeyword', 'agebetween', 'age', 'isfree', 'isparent', 'permanent', 'temporary', 'location', 'organizer', 'type', 'thema', 'targetaudience', 'facility', 'publicscope', 'locationtype', 'eventtype', 'actortype', 'municipal', 'ipe', 'misc', 'heading', 'daterange', 'date', 'datetype');

  if (in_array($params['action'], array('list', 'summary', 'report'))) {
    if ($params['type'] == 'actor') {
      $valid_queries = $valid_queries_common;
    }
    elseif (in_array($params['type'], array('event', 'production'))) {
      $valid_queries = $valid_queries_common + $valid_queries_events_productions;
    }
    $diff = array_diff(array_keys($params['query']), $valid_queries);
    if (!empty($diff)) {
      return FALSE;
    }
  }

  // providing a hook so other modules can restrict access to certain queries
  foreach (module_implements('cnapi_params_validate') as $module) {
    if (!module_invoke($module, 'cnapi_params_validate')) {
      return FALSE;
    }
  }

  return TRUE;
}

function _cnapi_params_expand_key(&$params) {
  // @todo implement this
}

function cnapi_params_sort(&$params) {
  ksort($params); // sorting $params by order 'action', 'type', 'query'
  ksort($params['query']); // sorting all queries

  // sorting all multivalue queries
  foreach ($params['query'] as $key => $value) {
    $parts = explode(';', $value);
    asort($parts);
    $params['query'][$key] = implode(';', $parts);
  }
}

function cnapi_params_hash($params) {
  cnapi_params_clean($params);
  cnapi_params_sort($params);
  return md5(serialize($params));
}

function cnapi_query_hash($params) {
  cnapi_params_clean($params);
  cnapi_params_sort($params);
  return md5(serialize($params['query']));
}

function cnapi_base_query_hash($params) {
  $base_params = $params;
  unset($base_params['query']['page']);
  unset($base_params['query']['pagelength']);
  unset($base_params['query']['sort']);
  return cnapi_query_hash($base_params);
}

function cnapi_url_a2p($url = '') {
  $params = array('action' => '', 'type' => '', 'query' => array());

  $url = ltrim($url, '/');
  extract(parse_url($url));

  // usually the url $query is the $query
  parse_str($query, $params['query']);

  // list, summary and report
  $match = array();
  if (preg_match_all('/^(events|actors|productions)\/(search|xmlview|report)$/i', $path, $match)) {
    $mapping = array(
      'search' => 'summary',
      'xmlview' => 'list',
      'report' => 'report',
    );
    $action = $match[2][0];
    $params['action'] = $mapping[$action];
    $params['type'] = substr($match[1][0], 0, -1);
  }

  // details
  $match = array();
  if (preg_match_all('/^(event|actor|production)\/([0-9a-zA-Z\-]*)$/i', $path, $match)) {
    $params['action'] = 'detail';
    $params['type'] = $match[1][0];
    $params['query']['cdbid'] = $match[2][0];
  }

  cnapi_params_clean($params);

  return $params;
}

function cnapi_url_p2a($params = array()) {
  cnapi_params_clean($params);

  $path = '';

  switch ($params['action']) {
    case 'detail':
      $path = $params['type'] . '/' . $params['query']['cdbid'];
      unset($params['query']['cdbid']);
      break;
    case 'list':
    case 'summary':
    case 'report':
      $mapping = array(
        'summary' => 'search',
        'list' => 'xmlview',
        'report' => 'report',
      );
      $action = $mapping[$params['action']];
      $path = $params['type'] . 's/' . $action;
  }

  $qs = http_build_query($params['query']);
  if (strlen($qs) > 0) $qs = '?' . $qs;
  return $path . $qs;
}