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

<?php if ($address) : ?>
  <dt>Waar</dt>
  <dd><?php print $address ?></dd>
<?php endif; ?>

<?php if ($reservation) : ?>
  <dt>Reservatie</dt>
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

<?php if ($images) : ?>
  <dt>Afbeeldingen</dt>
  <dd>
    <?php foreach ($images as $image) : ?>
      <?php print $image['image'] ?>
      <div>Copyright: <?php if (!empty($image['copyright'])) : ?><?php print $image['copyright'] ?><?php endif; ?></div>
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($coords) : ?>
  <dt>GPS</dt>
  <dd>
    <?php print $coords['lat'] ?>, <?php print $coords['lng'] ?>
  </dd>
<?php endif; ?>

</dl>