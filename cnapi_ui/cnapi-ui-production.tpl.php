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

<?php if ($directors) : ?>
  <dt>Regisseurs</dt>
  <dd>
    <?php foreach ($directors as $director) : ?>
      <?php print $director ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($actors) : ?>
  <dt>Acteurs</dt>
  <dd>
    <?php foreach ($actors as $actor) : ?>
      <?php print $actor ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($performers) : ?>
  <dt>Andere uitvoerders</dt>
  <dd>
    <?php foreach ($performers as $performer) : ?>
      <?php print $performer ?>,
    <?php endforeach; ?>
  </dd>
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

<?php if ($for_children) : ?>
  <dt>Vlieg</dt>
  <dd><?php print $vlieg_image ?></dd>
<?php endif; ?>

<?php if ($agefrom) : ?>
  <dt>Leeftijd vanaf</dt>
  <dd><?php print $agefrom ?></dd>
<?php endif; ?>

</dl>