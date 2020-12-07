
static int doing_logs_update = 0;
void  dmalloc_logs_update(time_t now)
{
  time_t elapsed;

  if (!libc_wrappers_initialized()) return;

  if (pthread_mutex_lock(&dmalloc_stats_mutex) == 0) {
    if (doing_logs_update) {
      pthread_mutex_unlock(&dmalloc_stats_mutex);
      return;
    }
    elapsed = now - dmalloc_stats.s_last_logupdate;
    if (elapsed >= 5) {
      doing_logs_update = 1;
      dmalloc_stats.s_last_logupdate = now;
    }
    pthread_mutex_unlock(&dmalloc_stats_mutex);
  } else {
    dmalloc_stats._s_errorlock++;
  }
  if (doing_logs_update) {
    dmalloc_log_stats();
    doing_logs_update = 0;
  }
