<div class="actor-summary">
  <h2><?php print $title ?></h2>

  <?php if ($thumbnail) : ?>
    <?php print $thumbnail ?>
  <?php endif; ?>

  <div class="short-description"><?php print $short_description ?></div>

  <dl>
    <?php if ($where) : ?>
      <dt class="where"><?php print t('Where')?></dd>
        <dd class="where"><?php print $where ?></dd>
    <?php endif; ?>
  </dl>
</div>