<?php

/**
 * @file
 * Handles some SEO optimisations for lists (contexts) and detail pages.
 */

/**
 * Preprocess variables for theme_link.
 *
 * @see theme_link()
 */
function template_preprocess_link(&$variables) {
  // Pase the link url as a Drupal cnapi request object.
  $request = cnapi_url_dua2dp(array('path' => $variables['path'], 'options' => $variables['options']));

  // Only act on cnapi list links.
  if ($request && isset($request['context'])) {
    // In case the url has more parameters than page and heading, we add rel=nofollow. We only want search engines to index lists by heading.
    if (count(array_diff(array_keys($request['query']), array('page', 'heading'))) > 0) {
      $variables['options']['attributes']['rel'] = 'nofollow';
    }

    // We remove the "active" class on all cnapi list links.
    // Because all cnapi list links have the same path (but different querystring), Drupal marks all list links as active on all pages.
    $classes = isset($variables['options']['attributes']['class']) ? $variables['options']['attributes']['class'] : array();

    $classes = array_diff($classes, array('active'));

    unset($variables['options']['attributes']['class']);
    if (!empty($classes)) {
      $variables['options']['attributes']['class'] = $classes;
    }
  }
}

/**
 * Put noindex and canonical metatags for SEO on relevant cnapi pages.
 * Since we only want search engines to index listings by heading, we put a "noindex" on all other listings (ex. ?heading=123&cityid=9000_GENT).
 * We put a canonical reference to the "heading" list for all listings that are listings by heading but also specify other query parameters (ex. ?heading=123&cityid=9000_GENT => ?heading=123)
 */
function cnapi_ui_set_canonical_and_noindex() {
  // Get active request.
  $request = cnapi_ui_get_active_request(FALSE);

  // We only act upon cnapi requests.
  if (!$request) {
    return;
  }

  // By default we allow robots to index and set no canonical.
  $noindex = FALSE;
  $canonical = FALSE;

  // If a context was set, we're on a list.
  if (isset($request['context'])) {
    // Fetch the context info
    $context = cnapi_ui_contexts('id', $request['context']);

    // And the query
    $query = $request['query'];

    // If we're on an event list, we look at headings
    if ($context['type'] == 'event') {
      if (count(array_diff(array_keys($request['query']), array('page', 'heading'))) > 0) {
        $noindex = TRUE;

        if (isset($request['query']['heading'])) {
          $canonical = cnapi_url_dp2dua(array('context' => $request['context'], 'query' => array('heading' => $request['query']['heading'])));
        }
      }
    }

    // On actor and production listings we only allow the "page" parameter in the querystring.
    else {
      if (count(array_diff(array_keys($request['query']), array('page'))) > 0) {
        $noindex = TRUE;

        // We also put a canonical to the actor or production listing without parameters.
        $canonical = cnapi_url_dp2dua(array('context' => $request['context']));
      }
    }
  }

  // We only allow indexing of actor pages that are "assets".
  if (isset($request['actor'])) {
    $actor = cnapi_get_actor($request['actor']);
    if (!$actor['asset']) {
      $noindex = TRUE;
    }
  }

  // Add the "noindex, follow" meta tag if needed.
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

  // Add the rel="canonical" tag if needed.
  if ($canonical) {
    $canonical['options']['absolute'] = TRUE;
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($canonical['path'], $canonical['options'])), TRUE);
  }
}

/**
 * Detail page urls have a "slug". We don't care about the actual value of it since we only look at the cdbid in the 4th path argument. So agenda/e/abc/123 is the same as agenda/e/xyz/123.
 * To not be marked as duplicate content, we redirect urls like this to the one and only correct slug version, based on the object's title.
 */
function cnapi_ui_seo_redirects() {
  // Get active request.
  $request = cnapi_ui_get_active_request();

  // We only act upon cnapi requests.
  if (!$request) {
    return;
  }

  // We only act upon detail cnapi page request.
  if (count(array_intersect(array_keys($request), array('actor', 'event', 'production'))) > 0) {
    // Fetch the object's info from the api.
    $object = cnapi_get(cnapi_url_dp2p($request));

    // Add the title to the current $request.
    if (isset($object['detail']['nl']['title'])) {
      $request['title'] = $object['detail']['nl']['title'];
    }
    else {
      $request['title'] = $object['title'];
    }

    // Convert the $request to a Drupal url array. This will result in the one and only correct representation of the url of the current request.
    $url = cnapi_url_dp2dua($request);

    module_load_include('inc', 'cnapi_ui', 'cnapi_ui.hacks');

    // Compare the $url path part of the correct representation with the path of the current request.
    if ($url['path'] != request_path_raw()) {
      cnapi_ui_goto($request);
    }
  }
}