<?php

/**
 * Preprocess variables for theme_link.
 *
 * @see theme_link()
 */
function template_preprocess_link(&$variables) {
  $request = cnapi_url_dua2dp(array('path' => $variables['path'], 'options' => $variables['options']));

  if (!empty($request) && isset($request['context'])) {
    if (count(array_diff(array_keys($request['query']), array('page', 'heading'))) > 0) {
      $variables['options']['attributes']['rel'] = 'nofollow';
    }

    $classes = isset($variables['options']['attributes']['class']) ? $variables['options']['attributes']['class'] : array();

    $classes = array_diff($classes, array('active'));

    unset($variables['options']['attributes']['class']);
    if (!empty($classes)) {
      $variables['options']['attributes']['class'] = $classes;
    }
  }
}

function cnapi_ui_set_canonical_and_noindex() {
  // get active request
  $request = cnapi_ui_get_active_request(FALSE);

  if (empty($request)) {
    return;
  }

  // by default we allow robots to index and set no canonical
  $noindex = FALSE;
  $canonical = FALSE;

  if (isset($request['context'])) {
    $context = cnapi_ui_contexts('id', $request['context']);

    $query = $request['query'];

    if ($context['type'] == 'event') {
      if (count(array_diff(array_keys($request['query']), array('page', 'heading'))) > 0) {
        $noindex = TRUE;

        if (isset($request['query']['heading'])) {
          $canonical = cnapi_url_dp2dua(array('context' => $request['context'], 'query' => array('heading' => $request['query']['heading'])));
        }
      }
    }
    else {
      if (count(array_diff(array_keys($request['query']), array('page'))) > 0) {
        $noindex = TRUE;

        $canonical = cnapi_url_dp2dua(array('context' => $request['context']));
      }
    }
  }

  if (isset($request['actor'])) {
    $actor = cnapi_get_actor($request['actor']);
    if (!$actor['asset']) {
      $noindex = TRUE;
    }
  }

  if ($noindex) {
    $element = array(
      '#tag' => 'meta',
      '#attributes' => array(
        'name' => 'robots',
        'content' => 'noindex, follow',
      ),
    );

    drupal_add_html_head($element);
  }

  if ($canonical) {
    $canonical['options']['absolute'] = TRUE;
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($canonical['path'], $canonical['options'])), TRUE);
  }
}

function cnapi_ui_seo_redirects() {
  // get active request
  $request = cnapi_ui_get_active_request();

  if (empty($request)) {
    return;
  }

  if (count(array_intersect(array_keys($request), array('actor', 'event', 'production'))) > 0) {
    $object = cnapi_get(cnapi_url_dp2p($request));

    if (isset($request['actor'])) {
      $request['title'] = $object['title'];
    }
    else {
      $request['title'] = $object['detail']['nl']['title'];
    }

    $url = cnapi_url_dp2dua($request);
    $uri = request_uri();
    $uri = substr($uri, strlen(base_path()), strlen($uri));

    if ($url['path'] != $uri) {
      cnapi_ui_goto($request);
    }
  }
}