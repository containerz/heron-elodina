// Copyright 2016 Twitter. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package backtype.storm.metric.api;

import java.util.HashMap;
import java.util.Map;

public class MultiCountMetric implements IMetric<Map<String, Long>> {
  private Map<String, CountMetric> value = new HashMap<>();

  public MultiCountMetric() {
  }

  public CountMetric scope(String key) {
    CountMetric val = value.get(key);
    if (val == null) {
      value.put(key, val = new CountMetric());
    }
    return val;
  }

  @Override
  public Map<String, Long> getValueAndReset() {
    Map<String, Long> ret = new HashMap<>();
    for (String key : value.keySet()) {
      ret.put(key, value.get(key).getValueAndReset());
    }
    return ret;
  }
}
