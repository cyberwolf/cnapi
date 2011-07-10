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

<?php if (isset($location)) : ?>
<dt>Where</dt>
<dd>
  <?php if (isset($location['link'])) : ?>
    <?php print $location['link'] ?>
  <?php else : ?>
    <?php print $location['title'] ?>
  <?php endif; ?><br />
  <?php print $location['address'] ?>
</dd>
<?php endif; ?>

<?php if (isset($organiser)) : ?>
<dt>Organiser</dt>
<dd>
  <?php if (isset($organiser['link'])) : ?>
    <?php print $organiser['link'] ?>
  <?php else : ?>
    <?php print $organiser['title'] ?>
  <?php endif; ?>
</dd>
<?php endif; ?>

<?php if (isset($reservation)) : ?>
  <dt>Reservation</dt>
  <dd>
  <?php if (isset($reservation['mail'])) : ?>
    Mail : <?php print $reservation['mail'] ?><br />
  <?php endif; ?>
  <?php if (isset($reservation['phone'])) : ?>
    Phone : <?php print $reservation['phone'] ?><br />
  <?php endif; ?>
  <?php if (isset($reservation['fax'])) : ?>
    Fax : <?php print $reservation['fax'] ?>
  <?php endif; ?>
  </dd>
<?php endif; ?>

<?php if (isset($contact)) : ?>
  <dt>Contact</dt>
  <dd>
  <?php if (isset($contact['mail'])) : ?>
    Mail : <?php print $contact['mail'] ?><br />
  <?php endif; ?>
  <?php if (isset($contact['phone'])) : ?>
    Phone : <?php print $contact['phone'] ?><br />
  <?php endif; ?>
  <?php if (isset($contact['fax'])) : ?>
    Fax : <?php print $contact['fax'] ?>
  <?php endif; ?>
  </dd>
<?php endif; ?>

<?php if ($when) : ?>
  <dt>When</dt>
  <dd><?php print $when ?></dd>
<?php endif; ?>

<?php if ($performers) : ?>
  <dt>Performers</dt>
  <dd><?php print $performers ?></dd>
<?php endif; ?>

<dt>Headings</dt>
<dd><?php print implode(', ', $headings) ?></dd>

<?php if ($images) : ?>
<dt>Images</dt>
<dd>
  <?php foreach ($images as $image) : ?>
    <?php print $image['image'] ?>
    <div>Copyright: <?php if ($image['copyright']) : ?><?php print $image['copyright'] ?><?php endif; ?></div>
  <?php endforeach; ?>
</dd>
<?php endif; ?>

<?php if ($price) : ?>
<dt>Price</dt>
<dd><?php print $price ?></dd>
<?php endif; ?>

<?php if ($for_children) : ?>
<dt>Vlieg</dt>
  <?php print $vlieg_image ?>
<?php endif; ?>

</dl>