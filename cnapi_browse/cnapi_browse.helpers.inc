<?php

function cnapi_browse_get_event_report($query = array()) {
  cnapi_ui_apply_context_query($query, 'event');

  $report = cnapi_get_report('event', $query);

  // movies as productions
  $movies_context = cnapi_ui_contexts('id', 'movie');

  if ($movies_context) {
    $movies_query = $query;
    cnapi_ui_remove_context_query($query, 'event');
    unset($query['heading']);
    cnapi_ui_apply_context_query($movies_query, 'movie');

    $movies = cnapi_get_productions($movies_query);

    $movie_item = NULL;
    if (isset($report['headings'][64])) {
      $movie_item = &$report['headings'][64];
    }
    elseif (isset($report['headings'][176]['children'][64])) {
      $movie_item = &$report['headings'][176]['children'][64];
    }

    if ($movie_item) {
      $movie_item['total'] = $movies['total'];
      $movie_item['link']['type'] = 'production';
      cnapi_ui_remove_context_query($movie_item['link']['query'], 'event');
      unset($movie_item['link']['query']['heading']);
      $movie_item['link']['context'] = 'movie';
    }
  }

  // children movies as productions
  $movies_children_context = cnapi_ui_contexts('id', 'movie_children');

  if ($movies_children_context) {
    $movies_children_query = $query;
    cnapi_ui_remove_context_query($query, 'event');
    unset($query['heading']);
    cnapi_ui_apply_context_query($movies_children_query, 'movie_children');

    $movies_children = cnapi_get_productions($movies_children_query);

    $movies_children_item = NULL;
    if (isset($report['headings'][98])) {
      $movies_children_item = &$report['headings'][98];
    }
    elseif (isset($report['headings'][179]['children'][98])) {
      $movies_children_item = &$report['headings'][179]['children'][98];
    }

    if ($movies_children_item) {
      $movies_children_item['total'] = $movies_children['total'];
      $movies_children_item['link']['type'] = 'production';
      cnapi_ui_remove_context_query($movies_children_item['link']['query'], 'event');
      unset($movies_children_item['link']['query']['heading']);
      $movies_children_item['link']['context'] = 'movie_children';
    }
  }

  return $report;
}