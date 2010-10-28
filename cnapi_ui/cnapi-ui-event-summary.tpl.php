<div class="event-summary">
  <h2><?php print $title ?></h2>

  <?php if ($thumbnail) : ?>
    <?php print $thumbnail ?>
  <?php endif; ?>

  <div class="short-description"><?php print $short_description ?></div>

  <dl>
    <?php if ($who) : ?>
      <dt class="who"><?php print t('Who')?></dt>
      <dd class="who"><?php print $who ?><dd>
    <?php endif; ?>
    <?php if ($where) : ?>
      <dt class="where"><?php print t('Where')?></dd>
        <dd class="where"><?php print $where ?></dd>
    <?php endif; ?>
    <?php if ($when) : ?>
      <dt class="when"><?php print t('When') ?></dt>
      <dd class="when"><?php print $when ?></dd>
    <?php endif; ?>
  </dl>
</div>