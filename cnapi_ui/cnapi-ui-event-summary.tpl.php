<div id="cnapi-object-<?php print $cdbid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <h2<?php print $title_attributes; ?>>
    <a href="<?php print $object_url; ?>"><?php print $title; ?></a>
  </h2>
  <?php print render($title_suffix); ?>

  <?php if ($for_children) : ?>
    <?php print $vlieg_image ?>
  <?php endif; ?>

	<?php if ($thumbnail) : ?>
		<?php print $thumbnail ?>
	<?php endif; ?>

	<?php if ($shortdescription) : ?>
	  <div class="description">
	    <?php print $shortdescription ?>
	  </div>
	<?php endif; ?>

	<?php if ($performers || $where || $when) : ?>
	  <dl>

	    <?php if ($performers) : ?>
	      <dt>Wie</dt>
	      <dd><?php print $performers ?></dd>
	    <?php endif; ?>

	    <?php if ($where) : ?>
	      <dt>Waar</dt>
	      <dd><?php print $where ?></dd>
	    <?php endif; ?>

	    <?php if ($when) : ?>
	      <dt>Wanneer</dt>
	      <dd><?php print $when ?></dd>
	    <?php endif; ?>

	  </dl>
	<?php endif; ?>

	<?php print $more_link ?>

</div>