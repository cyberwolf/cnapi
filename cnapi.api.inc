<?php

/**
 * Make a request to the Cultuurnet API using the defaults added by this module and others. The request will also be cleaned to remove redundant parameters before firing off the request.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An array representing the answer (@see _cnapi_get) from the Cultuurnet API in case of success. FALSE in case an error occured.
 */
function cnapi_get($request) {
  cnapi_request_add_defaults($request);
  cnapi_request_clean($request);

  return _cnapi_get($request);
}

/**
 * Make a request to the Cultuurnet API.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An array representing the answer (@see _cnapi_get) from the Cultuurnet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_raw($request) {
  return _cnapi_get($request);
}

/**
 * Request an event from the the Cultuurnet API.
 *
 * @param $cdib
 *   The CDBID of the requested event.
 * @param $related_events
 *   Specifies if events related to the requested event should be fetched too and how these should be formatted. The options are:
 *     - CNAPI_LIST_IGNORE : Do not fetch related events.
 *     - CNAPI_LIST_SUMMARY : Fetch a summary of all related events.
 *     - CNAPI_LIST_DETAIL : Fetch the full info of related events.
 *     - CNAPI_LIST_ID : Fetch the CDBIDs of related events.
 *
 * @return
 *   An array representing the event (@see _cnapi_get) from the Cultuurnet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_event($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['relatedevents'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }
  return cnapi_get($request);
}

/**
 * Request a production from the the Cultuurnet API.
 *
 * @param $cdib
 *   The CDBID of the requested production.
 * @param $related_events
 *   Specifies if events related to the requested production should be fetched too and how these should be formatted. The options are:
 *     - CNAPI_LIST_IGNORE : Do not fetch related events.
 *     - CNAPI_LIST_SUMMARY : Fetch a summary of all related events.
 *     - CNAPI_LIST_DETAIL : Fetch the full info of related events.
 *     - CNAPI_LIST_ID : Fetch the CDBIDs of related events.
 *
 * @return
 *   An array representing the production (@see _cnapi_get) from the Cultuurnet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_production($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['relatedevents'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }

  return cnapi_get($request);
}

/**
 * Request an actor from the the Cultuurnet API.
 *
 * @param $cdib
 *   The CDBID of the requested actor.
 *
 * @return
 *   An array representing the actor (@see _cnapi_get) from the Cultuurnet API in case of success. FALSE in case an error occured.
 */
function cnapi_get_actor($cdbid) {
  $request = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $cdbid));
  return cnapi_get($request);
}

function cnapi_get_events($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'event', 'query' => $query);

  return cnapi_get($request);
}

function cnapi_get_productions($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'production', 'query' => $query);

  return cnapi_get($request);
}

function cnapi_get_actors($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => ($action == CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail'), 'type' => 'actor', 'query' => $query);
  return cnapi_get($request);
}

function cnapi_get_report($type, $query = array()) {
  $request = array('action' => 'report', 'type' => $type, 'query' => $query);
  return cnapi_get($request);
}

function _cnapi_get($request) {
  if (!cnapi_request_validate($request)) {
    watchdog('cnapi', 'Invalid API request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
    return FALSE;
  }

  $result = NULL;

  $cid = cnapi_cache_cid($request);

  $cache = _cnapi_cache_get($request);

  if ($cache != NULL) {
    $result = $cache;
  }
  else {
    $result = cnapi_api_request($request);

    if ($result) {
      $result = _cnapi_parse($request, $result);

      if ($result) {
        if (in_array($request['action'], array('list_detail', 'list_summary'))) {
          _cnapi_cache_list_clean_by_total($request, $result['total']);
        }

        if ($request['action'] != 'detail') { // detail actions will be cached at parse level
          _cnapi_cache_set($request, $result);
        }
      }
      else {
        watchdog('cnapi', 'Failed parsing for request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
        return FALSE;
      }
    }
    else {
      watchdog('cnapi', 'Failed request !request.', array('!request' => serialize($request)), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  return $result;
}

function _cnapi_cache_list_clean_by_total($request, $total) {
  // setting the cache and purgin invalid items
  // goal is to get consistent paging
  // algorithm: save total for a result set to cache. if another page from the same set is requested an it has a different total, clear all pages from result set.
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid_base = sprintf('%s:%s', $request['type'], cnapi_base_query_hash($request));
    $cid_total = sprintf('%s:total', $cid_base);
    if ($cache_total = cache_get($cid_total)) { // what if number of results = 0?
      if ($cache_total != $total) {
        cache_clear_all($cid_base, _cnapi_cache_table($request), TRUE);
        cache_set($cid_total, $total, _cnapi_cache_table($request), _cnapi_cache_expires($request));
      }
    }
    else {
      cache_set($cid_total, $total, _cnapi_cache_table($request), _cnapi_cache_expires($request));
    }
  }
}

function _cnapi_cache_table($request) {
  switch ($request['action']) {
    case 'detail':
      return 'cache_cnapi_detail';
    default:
      return 'cache_cnapi';
  }
}

function _cnapi_cache_expires($request) {
  $var = 'cnapi_cache_lifetime';

  if ($request['action'] == 'detail') {
    if ($request['type'] == 'production') {
      $var = 'cnapi_cache_lifetime_production';
    }
    else {
      $var = 'cnapi_cache_lifetime_detail';
    }
  }

  $expire = CACHE_TEMPORARY;
  if (variable_get($var, 0) > 0) {
    $expire = $_SERVER['REQUEST_TIME'] + variable_get($var, 0);
  }

  return $expire;
}

function cnapi_cache_cid($request) {
  unset($request['query']['format']);

  switch ($request['action']) {
    case 'detail':
      return implode(':', array($request['type'], cnapi_query_hash($request), $request['query']['cdbid']));
    case 'report':
      return implode(':', array($request['type'], cnapi_base_query_hash($request), $request['action']));
    default:
      return implode(':', array($request['type'], cnapi_base_query_hash($request), $request['action'], $request['query']['pagelength'], $request['query']['page'], $request['query']['sort']));
  }
}

function _cnapi_cache_get($request) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($request);
    $table = _cnapi_cache_table($request);
    if ($cache = cache_get($cid, $table)) {
      return $cache->data;
    }
  }
  return NULL;
}

function _cnapi_cache_set($request, $object) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($request);
    $table = _cnapi_cache_table($request);
    $expires = _cnapi_cache_expires($request);
    cache_set($cid, $object, $table, $expires);
  }
}

function cnapi_api_request($request) {
  if (!isset($request['query']['format'])) {
    $request['query']['format'] = 'xml';
  }
  $url = cnapi_url_p2a($request);
  $url = rtrim(variable_get('cnapi_api_location', CNAPI_API_LOCATION), '/') . '/api/' . $url;
  return cnapi_http_request($url);
}

function cnapi_http_request($url, $method = 'GET', $data = array()) {
  $reponse = FALSE;

  _cnapi_timer('start', $url);

  $ch = curl_init($url);

  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_TIMEOUT, CNAPI_HTTP_REQUEST_TIMEOUT);
  curl_setopt($ch, CURLOPT_ENCODING, 'gzip, deflate');

  // support for proxy servers
  if (variable_get('cnapi_proxy_enabled', FALSE)) {
    curl_setopt($ch, CURLOPT_PROXY, trim(variable_get('cnapi_proxy_server', '')));
    curl_setopt($ch, CURLOPT_PROXYPORT, variable_get('cnapi_proxy_port', ''));
    curl_setopt($ch, CURLOPT_PROXYUSERPWD, sprintf('%s:%s', variable_get('cnapi_proxy_username', ''), variable_get('cnapi_proxy_password', '')));
  }

  // support for POST method
  if ($method == 'POST') {
    curl_setopt($ch, CURLOPT_POST, TRUE);
    if (!empty($data)) {
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    }
  }

  $response = curl_exec($ch);
  $curl_info = curl_getinfo($ch);

  curl_close($ch);

  _cnapi_timer('stop', $url);

  if ($curl_info['http_code'] != 200 || !$response) {
    if ($response) {
      if ($error = _cnapi_parse_error($response)) {
        _cnapi_set_last_request_error($error);
      }
      else {
        _cnapi_set_last_request_error(FALSE);
      }
    }
    else {
      _cnapi_set_last_request_error(FALSE);
    }
    watchdog('cnapi', 'Error doing request !request in cnapi_request_api_xml : Return code was !error', array('!request' => $url, '!error' => $curl_info['http_code']), WATCHDOG_ERROR);
    return FALSE;
  }
  else {
    _cnapi_set_last_request_error(FALSE);
    return $response;
  }
}

/**
 * Get detailed information on the error status of the last request made using cnapi_http_request.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An associative array containing detailed information on the last error in case of an error (that was returned with a detailed error description) .
 *   FALSE in case the last request was successfull or in case no detailed error message was returned.
 *
 *   Error format is:
 *     $error = array(
 *       'code' => 'AuthenticationFailed',
 *       'level' => 'ERROR',
 *       'message' => 'Authentication failed for test@test.net',
 *       'resource' => '/userpool/new',
 *     );
 */
function cnapi_get_last_request_error() {
  return _cnapi_set_last_request_error();
}

function _cnapi_set_last_request_error($error = NULL) {
  $last_error = &drupal_static(__FUNCTION__, NULL);

  if ($error !== NULL) {
    $last_error = $error;
  }

  return $last_error;
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

function _cnapi_default_values($key = NULL, $namespace = NULL) {
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

// only supports scalar defaults
// stuff like pagelength, page and sort are always specified although not necessary to get a meaningful cache id (cid still works if defaults change)
function cnapi_request_add_defaults(&$request) {
  $defaults = _cnapi_query_defaults($request);

  // setting defaults
  foreach ($defaults as $key => $value) {
    if (!isset($request['query'][$key])) {
      $request['query'][$key] = $value;
    }
  }
}

function cnapi_request_remove_defaults(&$request) {
  $defaults = _cnapi_query_defaults($request);

  // setting defaults
  foreach ($defaults as $key => $value) {
    if (isset($request['query'][$key]) && $request['query'][$key] == $value) {
      unset($request['query'][$key]);
    }
  }
}

function _cnapi_query_defaults($request) {
  // format
  $defaults['format'] = 'xml';

  // api key
  $defaults['key'] = trim(variable_get('cnapi_api_key', ''));

  // detail
  if ($request['action'] == 'detail' && in_array($request['type'], array('event', 'production'))) {
    $defaults['relatedevents'] = 'id';
  }

  // lists
  if (in_array($request['action'], array('list_detail', 'list_summary'))) {
    // page
    $defaults['page'] = 1;

    // calculating defaults
    $defaults = array_merge($defaults, _cnapi_default_values());

    foreach (array('event', 'production', 'actor') as $type) {
      if ($request['type'] == $type) {
        $namespace_defaults = array();
        foreach (_cnapi_default_values(NULL, $type) as $key => $val) {
          if ($val != -1) {
            $namespace_defaults[$key] = $val;
          }
        }
        $defaults = array_merge($defaults, $namespace_defaults);
      }
    }
  }

  drupal_alter('cnapi_request_defaults', $defaults, $request);

  return $defaults;
}

// cnapi_request_clean is very important in case of report because cache id depends on it, otherwise we get a different cache per page
// Does a best effort. : cleaning up for report, detail, ... but for "query" we don't care
function cnapi_request_clean(&$request) {
  $valid_query_detail = array('key', 'cdbid', 'relatedevents');
  $invalid_query_report = array('page', 'pagelength', 'sort');

  // clean detail requests
  if ($request['action'] == 'detail') {
    $request['query'] = array_intersect_key($request['query'], array_flip($valid_query_detail));
  }

  // clean report requests
  elseif ($request['action'] == 'report') {
    $request['query'] = array_diff_key($request['query'], array_flip($invalid_query_report));
  }
}

function cnapi_request_validate($request) {
  $valid_actions = array('list_detail', 'list_summary', 'detail', 'report');
  $valid_types = array('event', 'production', 'actor');

  // all requests should have a key
  if (!$request['query']['key']) {
    return FALSE;
  }

  // check if action is valid
  if (!in_array($request['action'], $valid_actions)) {
    return FALSE;
  }

  // check if type is valid
  if (!in_array($request['type'], $valid_types)) {
    return FALSE;
  }

  // reports not valid on actors
  if ($request['type'] == 'actor' && $request['action'] == 'report') {
    return FALSE;
  }

  // validate $query keys for list, summary and report
  $valid_queries_common = array('key', 'q', 'k', 'changedsince', 'zip', 'city', 'cityid', 'regio', 'latlng', 'format', 'sort', 'page', 'pagelength');
  $valid_queries_events_productions = array('locationkeyword', 'organiserkeyword', 'organiser', 'agebetween', 'age', 'isfree', 'isparent', 'permanent', 'temporary', 'location', 'agefrom', 'organizer', 'type', 'thema', 'targetaudience', 'facility', 'publicscope', 'locationtype', 'eventtype', 'actortype', 'municipal', 'ipe', 'misc', 'heading', 'daterange', 'date', 'datetype');

  if (in_array($request['action'], array('list_detail', 'list_summary', 'report'))) {
    if ($request['type'] == 'actor') {
      $valid_queries = $valid_queries_common;
    }
    elseif (in_array($request['type'], array('event', 'production'))) {
      $valid_queries = array_merge($valid_queries_common, $valid_queries_events_productions);
    }
    $diff = array_diff(array_keys($request['query']), $valid_queries);
    if (!empty($diff)) {
      return FALSE;
    }
  }

  // providing a hook so other modules can restrict access to certain queries
  foreach (module_implements('cnapi_request_validate') as $module) {
    if (!module_invoke($module, 'cnapi_request_validate')) {
      return FALSE;
    }
  }

  return TRUE;
}

function cnapi_request_sort(&$request) {
  ksort($request); // sorting $request by order 'action', 'type', 'query'
  ksort($request['query']); // sorting all queries

  // sorting all multivalue queries
  foreach ($request['query'] as $key => $value) {
    $parts = explode(';', $value);
    asort($parts);
    $request['query'][$key] = implode(';', $parts);
  }
}

/**
 * Calculate a hash value of an API request array.
 *
 * Uniquely identifies an API request. Requests are identical if they have the same action, type, query. Order of the query parts and values is ignored (@see cnapi_query_hash).
 *
 * @param $request
 *   An API request array (@see _cnapi_get).
 *
 * @return
 *   The hash value of the API request array.
 */
function cnapi_request_hash($request) {
  cnapi_request_clean($request);
  cnapi_request_sort($request);
  return md5(serialize($request));
}

/**
 * Calculate a hash value for the 'query' of an API request array.
 *
 * Uniquely identifies a 'query' of an API request. Order of the query parts and values is ignored. So array('page' => 1, 'q' => 'Some query', 'heading' => '1;2;3') has the same hash as array('q' => 'Some query', 'page' => 1, 'heading' => '2;3,1').
 *
 * @param $request
 *   An API request array (@see _cnapi_get).
 *
 * @return
 *   The hash value of the 'query' of the API request array.
 */
function cnapi_query_hash($request) {
  cnapi_request_clean($request);
  cnapi_request_sort($request);
  return md5(serialize($request['query']));
}

function cnapi_base_query_hash($request) {
  $base_request = $request;
  unset($base_request['query']['page']);
  unset($base_request['query']['pagelength']);
  unset($base_request['query']['sort']);
  return cnapi_query_hash($base_request);
}

function cnapi_url_a2p($url = '') {
  $request = array('action' => '', 'type' => '', 'query' => array());

  $url = ltrim($url, '/');
  extract(parse_url($url));

  // usually the url $query is the $query
  parse_str($query, $request['query']);

  // list, summary and report
  $match = array();
  if (preg_match_all('/^(events|actors|productions)\/(search|xmlview|report)$/i', $path, $match)) {
    $mapping = array(
      'search' => 'list_summary',
      'xmlview' => 'list_detail',
      'report' => 'report',
    );
    $action = $match[2][0];
    $request['action'] = $mapping[$action];
    $request['type'] = substr($match[1][0], 0, -1);
  }

  // details
  $match = array();
  if (preg_match_all('/^(event|actor|production)\/([0-9a-zA-Z\-]*)$/i', $path, $match)) {
    $request['action'] = 'detail';
    $request['type'] = $match[1][0];
    $request['query']['cdbid'] = $match[2][0];
  }

  cnapi_request_clean($request);

  return $request;
}

function cnapi_url_p2a($request = array()) {
  cnapi_request_clean($request);

  $path = '';

  switch ($request['action']) {
    case 'detail':
      $path = $request['type'] . '/' . $request['query']['cdbid'];
      unset($request['query']['cdbid']);
      break;
    case 'list_detail':
    case 'list_summary':
    case 'report':
      $mapping = array(
        'list_summary' => 'search',
        'list_detail' => 'xmlview',
        'report' => 'report',
      );
      $action = $mapping[$request['action']];
      $path = $request['type'] . 's/' . $action;
  }

  $qs = http_build_query($request['query'], '', '&');
  if (strlen($qs) > 0) $qs = '?' . $qs;
  return $path . $qs;
}