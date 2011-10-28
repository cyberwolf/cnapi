<div id="cnapi-object-<?php print $cdbid; ?>" class="<?php print $classes; ?> teaser clearfix"<?php print $attributes; ?>>

  <div class="teaser-image">
  
    <?php if ($thumbnail) : ?>
      <?php print $thumbnail ?>
    <?php endif; ?>

  </div>

  <div class="teaser-text">

    <?php print render($title_prefix); ?>
    <h<?php print $heading_level ?><?php print $title_attributes; ?>>
      <a href="<?php print $object_url; ?>"><?php print $title; ?></a>
    </h<?php print $heading_level ?>>
    <?php print render($title_suffix); ?>
  
	  <?php if ($headings) : ?>
      <div class="headings">
        <?php print $headings ?>
      </div>
    <?php endif; ?>

    <?php if ($for_children) : ?>
      <?php print $vlieg_image ?>
    <?php endif; ?>

    <?php if ($shortdescription) : ?>
      <div class="description">
        <?php print $shortdescription ?>
      </div>
    <?php endif; ?>

    <?php if ($performers || $where || $when) : ?>
      <dl class="clearfix">

        <?php if ($performers) : ?>
          <dt>Wie</dt>
          <dd><?php print $performers ?></dd>
        <?php endif; ?>

        <?php if ($where) : ?>
          <dt>Waar</dt>
          <dd><?php print $where ?></dd>
        <?php endif; ?>

        <?php if (true) : ?>
          <dt>Wanneer</dt>
          <dd><?php print $when ?></dd>
        <?php endif; ?>

      </dl>
    <?php endif; ?>

      <div class="teaser-actions">

      <?php print $more_link ?>

    </div>

  </div>

</div>