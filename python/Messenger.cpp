#include "Messenger.hpp"
#include <chrono>
#include <thread>

// Initiates the protobuf and reads in the file.
Messenger::Messenger(std::string file_name) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  f_name = file_name;
  std::fstream input(f_name.c_str(), std::ios::in | std::ios::binary);
  if (!input) {
    std::cout << f_name << ": File not found.  Creating a new file." << std::endl;
  } else if (!kodiak_messages.ParseFromIstream(&input)) {
    std::cerr << "Failed to parse messages." << std::endl;
  }
  // Clears the files contents after reading them in.
  // The file should be cleared when the class is destroyed but incase an error occured
  // It is redone.
  clear();
}

void Messenger::write(Bifurcation bif) {
  kodiak::Bifurcation *bif_mes = kodiak_messages.add_bifurcation();
  bif_mes->set_res(bif.get_res());
  bif_mes->set_prec(bif.get_prec());
  for (int i = 0; i < bif.get_var_names().size(); ++i) {
    kodiak::Var *var = bif_mes->add_var();
    var->set_name(bif.get_var_names()[i]);
    var->set_lb(bif.get_var_ints()[i].first);
    var->set_ub(bif.get_var_ints()[i].second);
  };
  for (int i = 0; i < bif.get_param_names().size(); ++i) {
    kodiak::Param *param = bif_mes->add_param();
    param->set_name(bif.get_param_names()[i]);
    param->set_lb(bif.get_param_ints()[i].first);
    param->set_ub(bif.get_param_ints()[i].second);
  };

  for (int i = 0; i < bif.get_cnstrs().size(); ++i) {
    kodiak::Cnstr *cnstr = bif_mes->add_cnstr();
    cnstr->set_cnstr(bif.get_cnstrs()[i]);
  };
  for (int i = 0; i < bif.get_dfeqs().size(); ++i) {
    kodiak::Dfeq *dfeq = bif_mes->add_dfeq();
    dfeq->set_dfeq(bif.get_dfeqs()[i]);
  };
  // Sets the m_done flag to true in the shared file, to let the other process done, that this is has finished writting.
  kodiak_messages.set_m_done(true);
  // Writes the results back to shared file.
  std::fstream output(f_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  if (!kodiak_messages.SerializeToOstream(&output)) {
    std::cerr << "Failed to write messages." << std::endl;
  }
}
// TODO find a way to factorize out shared parts of writting logic.
void Messenger::write(Problem prob) {
  std::fstream output(f_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  if (!kodiak_messages.SerializeToOstream(&output)) {
    std::cerr << "Failed to write messages." << std::endl;
  }
}

void Messenger::read(std::vector<Boxes> &answer, const char type) {
  // If kodiak has not produced an answer yet, this thread goes to sleep for 1000miliseconds.
  // Each iteration the file is read again and the condition checked.
  while (!kodiak_messages.k_done()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::fstream input(f_name.c_str(), std::ios::in | std::ios::binary);
    if (!input) {
      std::cout << f_name << ": File not found.  Creating a new file." << std::endl;
    } else if (!kodiak_messages.ParseFromIstream(&input)) {
      std::cerr << "Failed to parse messages." << std::endl;
    }
  }
  // b stands for bifurcation.
  // Read in the boxes from respective variable in the shared file.
  if (type == 'b') {
    for (int i = 0; i < kodiak_messages.bifans_size(); ++i) {
      const kodiak::Bif_Ans &bifans = kodiak_messages.bifans(i);
      for (int o = 0; o < bifans.boxtype_size(); ++o) {
        Boxes type;
        const kodiak::Box_Type boxtype = bifans.boxtype(o);
        for (int p = 0; p < boxtype.box_size(); ++p) {
          Box b;
          const kodiak::Box box = boxtype.box(p);
          for (int y = 0; y < box.interval_size(); ++y) {
            const kodiak::Interval interval = box.interval(y);
            b.push_back(std::make_pair(interval.lb(), interval.ub()));
          }
          type.push_back(b);
        }
        answer.push_back(type);
      }
    }
  }

};

void Messenger::clear() {
  kodiak_messages.Clear();
  std::fstream output(f_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  if (!kodiak_messages.SerializeToOstream(&output)) {
    std::cerr << "Failed to write messages." << std::endl;
  }
}

// Clears the file and shutsdown protobuf library
Messenger::~Messenger() {
  kodiak_messages.Clear();
  std::fstream output(f_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  if (!kodiak_messages.SerializeToOstream(&output)) {
    std::cerr << "Failed to write messages." << std::endl;
  }
  google::protobuf::ShutdownProtobufLibrary();
}
