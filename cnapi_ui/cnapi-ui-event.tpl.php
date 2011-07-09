<?php //if (!$is_node) : /* show title */; endif; ?>

<dl>
  
<dt>CDBID</dt>
<dd><?php print $cdbid ?></dd>

<dt>Title</dt>
<dd><?php print $title ?></dd>

<dt>Short description</dt>
<dd><?php print $shortdescription ?></dd>

<dt>Long description</dt>
<dd><?php print $longdescription ?></dd>

<dt>Headings</dt>
<dd><?php print implode(', ', $headings) ?></dd>

<dt>Images</dt>
<dd>
  <?php foreach ($images as $image) : ?>
    <?php print $image['image'] ?>
    <div>Copyright: <?php if ($image['copyright']) : ?><?php print $image['copyright'] ?><?php endif; ?></div>
  <?php endforeach; ?>
</dd>

<?php if ($price) : ?>
<dt>Price</dt>
<dd><?php print $price ?></dd>
<?php endif; ?>

<?php if ($is_vlieg) : ?>
<dt>Vlieg</dt>
  <?php print $vlieg_image ?>
<?php endif; ?>

</dl>