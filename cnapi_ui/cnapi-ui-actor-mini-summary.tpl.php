<div id="cnapi-object-<?php print $cdbid; ?>" class="<?php print $classes; ?> mini-teaser clearfix"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <h<?php print $heading_level ?><?php print $title_attributes; ?>>
    <a href="<?php print $object_url; ?>"><?php print $title; ?></a>
  </h<?php print $heading_level ?>>
  <?php print render($title_suffix); ?>

  <?php if ($where) : ?>
    <div class="where"><?php print $where ?></div>
  <?php endif; ?>

  <?php print $more_link ?>

</div>