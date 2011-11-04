<?php //if (!$is_node) : /* show title */; endif; ?>

<dl>

<dt>CDBID</dt>
<dd><?php print $cdbid ?></dd>

<dt>Titel</dt>
<dd><?php print $title ?></dd>

<?php if ($shortdescription) : ?>
  <dt>Korte omschrijving</dt>
  <dd><?php print $shortdescription ?></dd>
<?php endif; ?>

<?php if ($longdescription) : ?>
  <dt>Lange omschrijving</dt>
  <dd><?php print $longdescription ?></dd>
<?php endif; ?>

<?php if ($location) : ?>
  <dt>Waar</dt>
  <dd>
    <?php if (isset($location['link'])) : ?>
      <?php print $location['link'] ?>
    <?php else : ?>
      <?php print $location['title'] ?>
    <?php endif; ?><br />
    <?php print $location['address'] ?>
  </dd>
<?php endif; ?>

<?php if ($organiser && !$location_is_organiser) : ?>
  <dt>Organisatie</dt>
  <dd>
    <?php if (isset($organiser['link'])) : ?>
      <?php print $organiser['link'] ?>
    <?php else : ?>
      <?php print $organiser['title'] ?>
    <?php endif; ?>
  </dd>
<?php endif; ?>

<?php if ($reservation) : ?>
  <dt>Reservation</dt>
  <dd>
  <?php if (isset($reservation['mail'])) : ?>
    Mail: <?php print $reservation['mail'] ?><br />
  <?php endif; ?>
  <?php if (isset($reservation['phone'])) : ?>
    Telefoon: <?php print $reservation['phone'] ?><br />
  <?php endif; ?>
  <?php if (isset($reservation['fax'])) : ?>
    Fax: <?php print $reservation['fax'] ?>
  <?php endif; ?>
  </dd>
<?php endif; ?>

<?php if ($contact) : ?>
  <dt>Contact</dt>
  <dd>
  <?php if (isset($contact['mail'])) : ?>
    Mail: <?php print $contact['mail'] ?><br />
  <?php endif; ?>
  <?php if (isset($contact['phone'])) : ?>
    Telefoon: <?php print $contact['phone'] ?><br />
  <?php endif; ?>
  <?php if (isset($contact['fax'])) : ?>
    Fax: <?php print $contact['fax'] ?>
  <?php endif; ?>
  </dd>
<?php endif; ?>

<?php if ($when) : ?>
  <dt>Wanneer</dt>
  <dd><?php print $when ?></dd>
<?php endif; ?>

<?php if ($performers) : ?>
  <dt>Uitvoerder</dt>
  <dd>
    <?php foreach ($performers as $performer) : ?>
      <?php print $performer ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($headings) : ?>
  <dt>Rubrieken</dt>
  <dd>
    <?php foreach ($headings as $heading) : ?>
      <?php print $heading ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($is_only_french) : ?>
  <dt>Taal</dt>
  <dd>Frans</dd>
<?php endif; ?>

<?php if ($images) : ?>
  <dt>Afbeeldingen</dt>
  <dd>
    <?php foreach ($images as $image) : ?>
      <?php print $image['image'] ?>
      <div>Copyright: <?php if ($image['copyright']) : ?><?php print $image['copyright'] ?><?php endif; ?></div>
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($tickets) : ?>
  <dt>Tickets</dt>
  <dd>
    <?php foreach ($tickets as $ticket) : ?>
      <?php print $ticket ?>
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($price) : ?>
  <dt>Price</dt>
  <dd><?php print $price ?></dd>
<?php endif; ?>

<?php if ($for_children) : ?>
  <dt>Vlieg</dt>
  <dd><?php print $vlieg_image ?></dd>
<?php endif; ?>

<?php if ($agefrom) : ?>
  <dt>Leeftijd vanaf</dt>
  <dd><?php print $agefrom ?></dd>
<?php endif; ?>

<?php if ($targetaudiences) : ?>
  <dt>Doelgroep</dt>
  <dd>
    <?php foreach ($targetaudiences as $targetaudience) : ?>
      <?php print $targetaudience ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($facilities) : ?>
  <dt>Voorzieningen</dt>
  <dd>
    <?php foreach ($facilities as $facility) : ?>
      <?php print $facility ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($keywords) : ?>
  <dt>Tags</dt>
  <dd>
    <?php foreach ($keywords as $keyword) : ?>
      <?php print $keyword ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($links) : ?>
  <dt>Links</dt>
  <dd>
    <?php foreach ($links as $link) : ?>
      <?php print $link ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($coords) : ?>
  <dt>GPS</dt>
  <dd>
    <a href="http://maps.google.com/maps?hl=nl&amp;q=<?php print $coords['lat'] ?>, <?php print $coords['lng'] ?>"><img src="http://maps.google.com/maps/api/staticmap?center=<?php print $coords['lat'] ?>, <?php print $coords['lng'] ?>&amp;zoom=15&amp;markers=<?php print $coords['lat'] ?>, <?php print $coords['lng'] ?>&amp;size=300x200&amp;sensor=false"></a>
    <div><?php print $coords['lat'] ?>, <?php print $coords['lng'] ?></div>
  </dd>
<?php endif; ?>

</dl>