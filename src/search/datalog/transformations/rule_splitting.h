#ifndef SEARCH_DATALOG_TRANSFORMATIONS_RULE_SPLITTING_H_
#define SEARCH_DATALOG_TRANSFORMATIONS_RULE_SPLITTING_H_

#include "greedy_join.h"

#include "../datalog.h"

#include "../rules/rule_base.h"
#include "../rules/generic_rule.h"
#include "../rules/join.h"
#include "../rules/product.h"
#include "../rules/project.h"

#include <limits>

/*
 * Implement rule-splitting by Helmert (AIJ 2009).
 */

namespace  datalog {

std::unique_ptr<RuleBase> convert_into_project_rule(const std::unique_ptr<RuleBase> &rule, AnnotationGenerator &annotation_generator, int schema_id, const Task &task) {
    std::unique_ptr<Annotation> ann = annotation_generator(schema_id, task);
    std::unique_ptr<RuleBase> project_rule = std::make_unique<ProjectRule>(rule->get_weight(), rule->get_effect(), rule->get_conditions(),  std::move(ann));
    return project_rule;
}

std::unique_ptr<RuleBase> convert_into_product_rule(const std::unique_ptr<RuleBase> &rule, AnnotationGenerator &annotation_generator, int schema_id, const Task &task) {
    std::unique_ptr<Annotation> ann = annotation_generator(schema_id, task);
    std::unique_ptr<RuleBase> product_rule = std::make_unique<ProductRule>(rule->get_weight(),
                                                                           rule->get_effect(),
                                                                           rule->get_conditions(),
                                                                           std::move(ann));
    return product_rule;
}

void convert_into_join_rules(std::vector<std::unique_ptr<RuleBase>> &new_join_rules,
                             const std::unique_ptr<RuleBase> &rule,
                             AnnotationGenerator &annotation_generator,
                             int schema_id,
                             const Task &task) {
    JoinCost join_cost;
    //size_t idx1 = std::numeric_limits<size_t>::max();
    //size_t idx2 = std::numeric_limits<size_t>::max();
    while(rule->get_conditions().size() > 2) {
        for (size_t i = 0; i < rule->get_conditions().size() - 1; ++i) {
            for (size_t j = i+1; j < rule->get_conditions().size(); ++j) {
                JoinCost cost = compute_join_cost(rule, rule->get_conditions()[i], rule->get_conditions()[j]);
                if (cost < join_cost) {
                    join_cost = cost;
                    //idx1 = i;
                    //idx2 = j;
                }
            }
        }
        break; // TODO Remove when not testing it
    }
    std::unique_ptr<Annotation> ann = annotation_generator(schema_id, task);
    std::unique_ptr<RuleBase> join_rule = std::make_unique<JoinRule>(rule->get_weight(), rule->get_effect(), rule->get_conditions(), std::move(ann));
    join_rule->update_variable_source_table(rule->get_variable_source_table());

    new_join_rules.push_back(std::move(join_rule));
}

bool is_product_rule(const std::unique_ptr<RuleBase> &rule) {
    std::set<int> vars;
    for (const auto &condition : rule->get_conditions()) {
        for (const auto &arg : condition.get_arguments()) {
            if (!arg.is_object()) {
                if (vars.find(arg.get_index()) != vars.end()) return false;
                vars.insert(arg.get_index());
            }
        }
    }
    return true;
}

// Split a single rule
void split_rules(std::vector<std::unique_ptr<RuleBase>> &rules, AnnotationGenerator &annotation_generator, const Task &task) {
    std::vector<std::unique_ptr<RuleBase>> new_rules;
    // Store action schemas of all generic rules first
    std::vector<int> schemas;
    for (const auto &rule : rules) {
        GenericRule *action_rule_base = dynamic_cast<GenericRule *>(rule.get());
        schemas.push_back(action_rule_base->get_schema_index());
    }

    size_t rule_counter = 0;
    for (auto &rule : rules) {
        if (rule->get_conditions().size() == 1) {
            new_rules.push_back(convert_into_project_rule(rule, annotation_generator, schemas[rule_counter], task));
        }
        else {
            if (is_product_rule(rule)) {
                new_rules.push_back(convert_into_product_rule(rule, annotation_generator, schemas[rule_counter], task));
            }
            else {
                std::vector<std::unique_ptr<RuleBase>> new_join_rules;
                convert_into_join_rules(new_join_rules, rule, annotation_generator, schemas[rule_counter], task);
                for (auto &r : new_join_rules) {
                    new_rules.push_back(std::move(r));
                }
            }
        }
        rule_counter++;
    }

    rules = std::move(new_rules);
}

}

#endif //SEARCH_DATALOG_TRANSFORMATIONS_RULE_SPLITTING_H_
