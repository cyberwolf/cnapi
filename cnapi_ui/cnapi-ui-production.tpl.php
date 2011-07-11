<?php //if (!$is_node) : /* show title */; endif; ?>

<dl>

<dt>CDBID</dt>
<dd><?php print $cdbid ?></dd>

<dt>Title</dt>
<dd><?php print $title ?></dd>

<?php if ($shortdescription) : ?>
  <dt>Short description</dt>
  <dd><?php print $shortdescription ?></dd>
<?php endif; ?>

<?php if ($longdescription) : ?>
  <dt>Long description</dt>
  <dd><?php print $longdescription ?></dd>
<?php endif; ?>

<?php if ($directors) : ?>
  <dt>Directors</dt>
  <dd>
    <?php foreach ($directors as $director) : ?>
      <?php print $director ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($actors) : ?>
  <dt>Actors</dt>
  <dd>
    <?php foreach ($actors as $actor) : ?>
      <?php print $actor ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($performers) : ?>
  <dt>Other performers</dt>
  <dd>
    <?php foreach ($performers as $performer) : ?>
      <?php print $performer ?>,
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

<?php if ($images) : ?>
  <dt>Images</dt>
  <dd>
    <?php foreach ($images as $image) : ?>
      <?php print $image['image'] ?>
      <div>Copyright: <?php if ($image['copyright']) : ?><?php print $image['copyright'] ?><?php endif; ?></div>
    <?php endforeach; ?>
  </dd>
<?php endif; ?>

</dl>