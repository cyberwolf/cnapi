<?php foreach ($movies as $province => $locations) : ?>
  <div class="movie-province">
    <h2><?php print $province ?></h2>
    <?php foreach ($locations as $location => $events) : ?>
      <div class="movie-location">
        <h3>
          <span class="location-name">
            <?php print cnapi_url_dp2dul($events[0]['location']['actor']['detail']['nl']['title'], array('actor' => $events[0]['location']['actor']['cdbid'], 'title' => $events[0]['location']['actor']['detail']['nl']['title'])); ?>
          </span>
          <br />
          <span class="location-city">
            <?php print $events[0]['location']['address']['city'] ?>
          </span>
        </h3>
        <?php foreach ($events as $event) : ?>
          <div class="event">
            <h4><?php print cnapi_url_dp2dul($event['detail']['nl']['title'], array('event' => $event['cdbid'])); ?></h4>
            <p><?php print $event['detail']['nl']['calendarsummary'] ?></p>
          </div>
        <?php endforeach; ?>
        <p><?php print cnapi_url_dp2dul('Andere films in deze zaal', array('actor' => $events[0]['location']['actor']['cdbid'], 'title' =>  $events[0]['location']['actor']['detail']['nl']['title'])); ?></p>
      </div>
    <?php endforeach; ?>
  </div>
<?php endforeach; ?>