<?php
// $Id$

function cnapi_get($request) {
  cnapi_request_add_defaults($request);
  cnapi_request_clean($request);

  return _cnapi_get($request);
}

function cnapi_get_raw($request) {
  return _cnapi_get($request);
}

function cnapi_get_event($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'event', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['related_events'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }
  return cnapi_get($request);
}

function cnapi_get_production($cdbid, $related_events = CNAPI_LIST_IGNORE) {
  $request = array('action' => 'detail', 'type' => 'production', 'query' => array('cdbid' => $cdbid));
  if ($related_events != CNAPI_LIST_IGNORE) {
    $request['query']['related_events'] = ($related_events == CNAPI_LIST_SUMMARY ? 'list' : 'detail');
  }
  return cnapi_get($request);
}

function cnapi_get_actor($cdbid) {
  $request = array('action' => 'detail', 'type' => 'actor', 'query' => array('cdbid' => $cdbid));
  return cnapi_get($request);
}

function cnapi_get_events($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail', 'type' => 'event', 'query' => $query);
  return cnapi_get($request);
}

function cnapi_get_productions($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail', 'type' => 'production', 'query' => $query);
  return cnapi_get($request);
}

function cnapi_get_actors($query = NULL, $action = CNAPI_LIST_SUMMARY) {
  $request = array('action' => CNAPI_LIST_SUMMARY ? 'list_summary' : 'list_detail', 'type' => 'actor', 'query' => $query);
  return cnapi_get($request);
}

function cnapi_get_report($type, $query = NULL) {
  $request = array('action' => 'report', 'type' => $type, 'query' => $query);
  return cnapi_get($request);
}

function _cnapi_get($request = NULL) {
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

      if (in_array($request['action'], array('list_detail', 'list_summary'))) {
        _cnapi_cache_list_clean_by_total($request, $result['total']);
      }

      _cnapi_cache_set($request, $result);
    }
  }

  return $result;
}

function _cnapi_parse($request, $result) {
  $data = array();
  
  $result = new SimpleXMLElement($result);
  $namespaces = $result->getDocNamespaces(); 
  $result->registerXPathNamespace('c', $namespaces['']); 
  
  if (in_array($request['action'], array('list_detail', 'list_summary'))) {
    $data['total'] = _cnapi_xpath_str($result, '/c:c/c:nofrecords');
  }
  
  if ($request['action'] == 'summary') {
    $items_path = '/c:cdbxml/c:list/c:item';
    if ($request['type'] == 'production') {
      $items_path = '/c:cdbxml/c:production/c:item';
    }
    
    foreach ($result->xpath($items_path) as $item) {
      $object = array();
      
      $object['cdbid'] = _cnapi_xpath_str($item, '@cdbid');
      $object['title'] = _cnapi_xpath_str($item, '@title');
      $object['shortdescription'] = _cnapi_xpath_str($item, '@shortdescription');
      $object['thumbnail'] = _cnapi_xpath_str($item, '@thumbnail');
      $object['created'] = strtotime(_cnapi_xpath_str($item, '@created'));
      
      if (in_array($request['type'], array('event', 'actor'))) {
        $object['city'] = _cnapi_xpath_str($item, '@city');
        $object['zip'] = _cnapi_xpath_str($item, '@zip');
        $object['address'] = _cnapi_xpath_str($item, '@address');
        $latlng = explode(';', _cnapi_xpath_str($item, '@latlng'));
        $object['lat'] = $latlng[0];
        $object['lng'] = $latlng[1];
      }
      
      if ($request['type'] == 'event') {
        $object['calendarsummary'] = _cnapi_xpath_str($item, '@calendarsummary');
        $object['headingid'] = _cnapi_xpath_int($item, '@headingid', TRUE);
        $object['locationid'] = _cnapi_xpath_str($item, '@locationid');
        $object['agefrom'] = _cnapi_xpath_int($item, '@agefrom');
        $object['performers'] = _cnapi_xpath_str($item, '@performers', TRUE);
        $object['available_to'] = strtotime(_cnapi_xpath_str($item, '@available_to'));
      }
      
      if ($request['type'] == 'production') {
        $object['agefrom'] = _cnapi_xpath_int($item, '@agefrom');
        $object['performers'] = _cnapi_xpath_str($item, '@performers', TRUE);
      }

      $data['data'][] = $object;
    }
  }
  
  return $data;
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
        cache_set($cid_total, $total, _cnapi_cache_table($request));
      }
    }
    else {
      cache_set($cid_total, $total, _cnapi_cache_table($request));
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
    $var = 'cnapi_cache_lifetime_detail';
  }

  $expire = CACHE_TEMPORARY;
  if (variable_get($var, 0) > 0) {
    $expire = $_SERVER['REQUEST_TIME'] + variable_get($var, 0);
  }

  // @improvement add hook so other modules can alter this

  return $expire;
}

function cnapi_cache_cid($request) {
  unset($request['query']['format']);

  _cnapi_request_expand_key($request);
  unset($request['query']['key']);

  switch ($request['action']) {
    case 'detail':
      return implode(':', array($request['type'], cnapi_query_hash($request), $param['query']['cdbid']));
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

// remove_defaults should be part of ui module, we only deal with api stuff here
// stuff like pagelength, page and sort are always specified although not necessary to get a meaningful cache id (cid still works if defaults change)
function cnapi_request_add_defaults(&$request) {
  // api key
  if (!isset($request['query']['key'])) {
    $request['query']['key'] = variable_get('cnapi_api_key', '');
  }

  if (in_array($request['action'], array('list_detail', 'list_summary'))) {
    // page
    if (!isset($request['query']['page'])) {
      $request['query']['page'] = 1;
    }

    // calculating defaults
    $defaults = _cnapi_default_values();

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

    // setting defaults
    foreach ($defaults as $key => $value) {
      if (!isset($request['query'][$key])) {
        $request['query'][$key] = $value;
      }
    }
  }

  drupal_alter('cnapi_request_defaults', $request);

  return $request;
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
  $valid_queries_events_productions = array('locationkeyword', 'agebetween', 'age', 'isfree', 'isparent', 'permanent', 'temporary', 'location', 'organizer', 'type', 'thema', 'targetaudience', 'facility', 'publicscope', 'locationtype', 'eventtype', 'actortype', 'municipal', 'ipe', 'misc', 'heading', 'daterange', 'date', 'datetype');

  if (in_array($request['action'], array('list_detail', 'list_summary', 'report'))) {
    if ($request['type'] == 'actor') {
      $valid_queries = $valid_queries_common;
    }
    elseif (in_array($request['type'], array('event', 'production'))) {
      $valid_queries = $valid_queries_common + $valid_queries_events_productions;
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

function _cnapi_request_expand_key(&$request) {
  // @todo implement this
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

function cnapi_request_hash($request) {
  cnapi_request_clean($request);
  cnapi_request_sort($request);
  return md5(serialize($request));
}

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

  $qs = http_build_query($request['query']);
  if (strlen($qs) > 0) $qs = '?' . $qs;
  return $path . $qs;
}