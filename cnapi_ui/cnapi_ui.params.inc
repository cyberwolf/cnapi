<?php

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
      if (isset($context['query'])) {
        foreach ($context['query'] as $key => $value) {
          if (isset($query[$key]) && $query[$key] == $value) { // @todo fix for value overrides
            unset($query[$key]);
          }
        }
      }
      
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
  $request = array();
  
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
    $contexts = cnapi_ui_contexts('path');
    
    $context = NULL;
    if (isset($contexts[$path])) {
      $context = $contexts[$path];
      
      $request['context'] = $context['id'];
      $request['query'] = $url['options']['query'];
    }
  }
  
  // @todo filter unwanted query strings (vb. utm_source, ...)
  
  return $request;
}

function cnapi_url_dp2p($request) {
  
  if (isset($request['context'])) {
    $request['action'] = CNAPI_LIST_SUMMARY;
    
    $context = cnapi_ui_contexts('id', $request['context']);
    
    unset($request['context']);
    $request['type'] = $context['type'];

    if ($query = $context['query']) {
      foreach ($query as $key => $value) {
        $request['query'][$key] = $value;
      }
    }
    
    if (isset($request['query']['page'])) {
      $request['query']['page']++;
    }
    
    if (isset($request['query']['query'])) {
      $request['query']['q'] = $request['query']['query'];
      unset($request['query']['query']);
    }
  }
  else {
    $request['action'] = 'detail';
    if (isset($request['event'])) {
      $request['type'] = 'event';
    }
    elseif (isset($request['actor'])) {
      $request['type'] = 'actor';
    }
    elseif (isset($request['production'])) {
      $request['type'] = 'production';
    }
    
    $type = $request['type'];
    $request['query']['cdbid'] = $request[$type];
  }
  
  return $request;
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
  if ($request['action'] == 'detail') {
    $type = $request['type'];
    $request[$type] = $request['query']['cdbid'];
    unset($request['action']);
    unset($request['query']);
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
    
    $result['context'] = $context;
    $result['query'] = $request['query'];
  }
  
  return $result;
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