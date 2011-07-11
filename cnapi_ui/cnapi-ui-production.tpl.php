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