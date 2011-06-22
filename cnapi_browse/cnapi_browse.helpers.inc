<?php

function cnapi_browse_get_event_report($query = array()) {
  $events_context = cnapi_ui_contexts('id', 'event');
  
  // @todo write a helper that combines a context query and an override (can be used in .params too)
  if (isset($events_context['query'])) {
    foreach ($events_context['query'] as $key => $value) {
      $query[$key] = $value;
    }
  }
  
  $report = cnapi_get_report('event', $query);
  
  $movies_context = cnapi_ui_contexts('id', 'movie');
  if ($movies_context) {
    $movies = cnapi_get_productions($movies_context['query']);
    if (isset($report['headings'][64])) {
      $report['headings'][64]['total'] = $movies['total'];
    }
    if (isset($report['headings'][176][64])) {
      $report['headings'][176][64]['total'] = $movies['total'];
    }
  }
  
  $movies_children_context = cnapi_ui_contexts('id', 'movie_children');
  if ($movies_children_context) {
    $movies_children = cnapi_get_productions($movies_children_context['query']);
    if (isset($report['headings'][98])) {
      $report['headings'][98]['total'] = $movies['total'];
    }
    if (isset($report['headings'][176][98])) {
      $report['headings'][176][98]['total'] = $movies['total'];
    }
  }
  
  return $report;
}