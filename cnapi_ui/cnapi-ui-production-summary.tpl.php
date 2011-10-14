<div id="cnapi-object-<?php print $cdbid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <h<?php print $heading_level ?><?php print $title_attributes; ?>>
    <a href="<?php print $object_url; ?>"><?php print $title; ?></a>
  </h<?php print $heading_level ?>>
  <?php print render($title_suffix); ?>

  <?php if ($thumbnail) : ?>
    <?php print $thumbnail ?>
  <?php endif; ?>

  <?php if ($shortdescription) : ?>
    <div class="description">
      <?php print $shortdescription ?>
    </div>
  <?php endif; ?>

  <?php if ($performers) : ?>
    <dl>

      <?php if ($performers) : ?>
        <dt>Wie</dt>
        <dd><?php print $performers ?></dd>
      <?php endif; ?>

    </dl>
  <?php endif; ?>

  <?php print $more_link ?>

</div>