/*
 * ITree.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: felicia
 */

#include "ITree.h"
#include "TimingSolver.h"
#include "Dependency.h"

#include <klee/Expr.h>
#include <klee/Solver.h>
#include <klee/util/ExprPPrinter.h>
#include <fstream>
#include <vector>

using namespace klee;

// Interpolation is enabled by default
bool InterpolationOption::interpolation = true;

/**/

unsigned long SearchTree::nextNodeId = 1;

SearchTree *SearchTree::instance = 0;

std::string SearchTree::PrettyExpressionBuilder::bvOne(unsigned width) {}
std::string SearchTree::PrettyExpressionBuilder::bvZero(unsigned width) {}
std::string SearchTree::PrettyExpressionBuilder::bvMinusOne(unsigned width) {}
std::string SearchTree::PrettyExpressionBuilder::bvConst32(unsigned width,
                                                           uint32_t value) {}
std::string SearchTree::PrettyExpressionBuilder::bvConst64(unsigned width,
                                                           uint64_t value) {}
std::string SearchTree::PrettyExpressionBuilder::bvZExtConst(unsigned width,
                                                             uint64_t value) {}
std::string SearchTree::PrettyExpressionBuilder::bvSExtConst(unsigned width,
                                                             uint64_t value) {}

std::string SearchTree::PrettyExpressionBuilder::bvBoolExtract(std::string expr,
                                                               int bit) {}
std::string SearchTree::PrettyExpressionBuilder::bvExtract(std::string expr,
                                                           unsigned top,
                                                           unsigned bottom) {}
std::string SearchTree::PrettyExpressionBuilder::eqExpr(std::string a,
                                                        std::string b) {}

// logical left and right shift (not arithmetic)
std::string SearchTree::PrettyExpressionBuilder::bvLeftShift(std::string expr,
                                                             unsigned shift) {}
std::string SearchTree::PrettyExpressionBuilder::bvRightShift(std::string expr,
                                                              unsigned shift) {}
std::string
SearchTree::PrettyExpressionBuilder::bvVarLeftShift(std::string expr,
                                                    std::string shift) {}
std::string
SearchTree::PrettyExpressionBuilder::bvVarRightShift(std::string expr,
                                                     std::string shift) {}
std::string
SearchTree::PrettyExpressionBuilder::bvVarArithRightShift(std::string expr,
                                                          std::string shift) {}

// Some STP-style bitvector arithmetic
std::string SearchTree::PrettyExpressionBuilder::bvMinusExpr(
    unsigned width, std::string minuend, std::string subtrahend) {}
std::string SearchTree::PrettyExpressionBuilder::bvPlusExpr(
    unsigned width, std::string augend, std::string addend) {}
std::string SearchTree::PrettyExpressionBuilder::bvMultExpr(
    unsigned width, std::string multiplacand, std::string multiplier) {}
std::string SearchTree::PrettyExpressionBuilder::bvDivExpr(
    unsigned width, std::string dividend, std::string divisor) {}
std::string SearchTree::PrettyExpressionBuilder::sbvDivExpr(
    unsigned width, std::string dividend, std::string divisor) {}
std::string SearchTree::PrettyExpressionBuilder::bvModExpr(
    unsigned width, std::string dividend, std::string divisor) {}
std::string SearchTree::PrettyExpressionBuilder::sbvModExpr(
    unsigned width, std::string dividend, std::string divisor) {}
std::string SearchTree::PrettyExpressionBuilder::notExpr(std::string expr) {}
std::string SearchTree::PrettyExpressionBuilder::bvNotExpr(std::string expr) {}
std::string SearchTree::PrettyExpressionBuilder::andExpr(std::string lhs,
                                                         std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::bvAndExpr(std::string lhs,
                                                           std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::orExpr(std::string lhs,
                                                        std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::bvOrExpr(std::string lhs,
                                                          std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::iffExpr(std::string lhs,
                                                         std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::bvXorExpr(std::string lhs,
                                                           std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::bvSignExtend(std::string src,
                                                              unsigned width) {}

// Some STP-style array domain interface
std::string SearchTree::PrettyExpressionBuilder::writeExpr(std::string array,
                                                           std::string index,
                                                           std::string value) {}
std::string SearchTree::PrettyExpressionBuilder::readExpr(std::string array,
                                                          std::string index) {}

// ITE-expression constructor
std::string SearchTree::PrettyExpressionBuilder::iteExpr(
    std::string condition, std::string whenTrue, std::string whenFalse) {}

// Bitvector length
int SearchTree::PrettyExpressionBuilder::getBVLength(std::string expr) {}

// Bitvector comparison
std::string SearchTree::PrettyExpressionBuilder::bvLtExpr(std::string lhs,
                                                          std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::bvLeExpr(std::string lhs,
                                                          std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::sbvLtExpr(std::string lhs,
                                                           std::string rhs) {}
std::string SearchTree::PrettyExpressionBuilder::sbvLeExpr(std::string lhs,
                                                           std::string rhs) {}

std::string SearchTree::PrettyExpressionBuilder::existsExpr(std::string body) {}

std::string SearchTree::PrettyExpressionBuilder::constructAShrByConstant(
    std::string expr, unsigned shift, std::string isSigned) {}
std::string SearchTree::PrettyExpressionBuilder::constructMulByConstant(
    std::string expr, unsigned width, uint64_t x) {}
std::string SearchTree::PrettyExpressionBuilder::constructUDivByConstant(
    std::string expr_n, unsigned width, uint64_t d) {}
std::string SearchTree::PrettyExpressionBuilder::constructSDivByConstant(
    std::string expr_n, unsigned width, uint64_t d) {}

std::string
SearchTree::PrettyExpressionBuilder::getInitialArray(const Array *os) {}
std::string
SearchTree::PrettyExpressionBuilder::getArrayForUpdate(const Array *root,
                                                       const UpdateNode *un) {}

std::string
SearchTree::PrettyExpressionBuilder::constructActual(ref<Expr> e,
                                                     int *width_out) {}
std::string SearchTree::PrettyExpressionBuilder::construct(ref<Expr> e,
                                                           int *width_out) {}

std::string SearchTree::PrettyExpressionBuilder::buildVar(const char *name,
                                                          unsigned width) {}
std::string SearchTree::PrettyExpressionBuilder::buildArray(
    const char *name, unsigned indexWidth, unsigned valueWidth) {}

std::string SearchTree::PrettyExpressionBuilder::getTrue() {}
std::string SearchTree::PrettyExpressionBuilder::getFalse() {}
std::string SearchTree::PrettyExpressionBuilder::getTempVar(Expr::Width w) {}
std::string
SearchTree::PrettyExpressionBuilder::getInitialRead(const Array *os,
                                                    unsigned index) {}

SearchTree::PrettyExpressionBuilder::PrettyExpressionBuilder() {}

SearchTree::PrettyExpressionBuilder::~PrettyExpressionBuilder() {}

std::string SearchTree::PrettyExpressionBuilder::construct(ref<Expr> e) {}

std::string SearchTree::recurseRender(const SearchTree::Node *node) {
  std::ostringstream stream;

  stream << "Node" << node->nodeId;
  std::string sourceNodeName = stream.str();

  stream << " [shape=record,label=\"{" << node->nodeId << ": "
         << node->iTreeNodeId << "\\l";
  for (std::map<PathCondition *, std::pair<std::string, bool> >::const_iterator
           it = node->pathConditionTable.begin(),
           itEnd = node->pathConditionTable.end();
       it != itEnd; ++it) {
    stream << (it->second.first);
    if (it->second.second)
      stream << " (I)";
    stream << "\\l";
  }
  if (node->subsumed) {
    stream << "(subsumed)\\l";
  }
  if (node->falseTarget || node->trueTarget)
    stream << "|{<s0>F|<s1>T}";
  stream << "}\"];\n";

  if (node->falseTarget) {
    stream << sourceNodeName << ":s0 -> Node" << node->falseTarget->nodeId
           << ";\n";
  }
  if (node->trueTarget) {
    stream << sourceNodeName + ":s1 -> Node" << node->trueTarget->nodeId
           << ";\n";
  }
  if (node->falseTarget) {
    stream << recurseRender(node->falseTarget);
  }
  if (node->trueTarget) {
    stream << recurseRender(node->trueTarget);
  }
  return stream.str();
}

std::string SearchTree::render() {
  std::string res("");

  // Simply return empty string when root is undefined
  if (!root)
    return res;

  std::ostringstream stream;
  for (std::map<SearchTree::Node *, SearchTree::Node *>::iterator
           it = subsumptionEdges.begin(),
           itEnd = subsumptionEdges.end();
       it != itEnd; ++it) {
    stream << "Node" << it->first->nodeId << " -> Node" << it->second->nodeId
           << " [style=dashed];\n";
  }

  res = "digraph search_tree {\n";
  res += recurseRender(root);
  res += stream.str();
  res += "}\n";
  return res;
}

SearchTree::SearchTree(ITreeNode *_root) {
  root = SearchTree::Node::createNode(_root->getNodeId());
  itreeNodeMap[_root] = root;
}

SearchTree::~SearchTree() {
  if (root)
    delete root;

  itreeNodeMap.clear();
}

void SearchTree::addChildren(ITreeNode *parent, ITreeNode *falseChild,
                             ITreeNode *trueChild) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  SearchTree::Node *parentNode = instance->itreeNodeMap[parent];
  parentNode->falseTarget =
      SearchTree::Node::createNode(falseChild->getNodeId());
  parentNode->trueTarget = SearchTree::Node::createNode(trueChild->getNodeId());
  instance->itreeNodeMap[falseChild] = parentNode->falseTarget;
  instance->itreeNodeMap[trueChild] = parentNode->trueTarget;
}

void SearchTree::setCurrentNode(ITreeNode *iTreeNode,
                                const uintptr_t programPoint) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  SearchTree::Node *node = instance->itreeNodeMap[iTreeNode];
  node->iTreeNodeId = programPoint;
  node->nodeId = nextNodeId++;
}

void SearchTree::markAsSubsumed(ITreeNode *iTreeNode,
                                SubsumptionTableEntry *entry) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  SearchTree::Node *node = instance->itreeNodeMap[iTreeNode];
  node->subsumed = true;
  SearchTree::Node *subsuming = instance->tableEntryMap[entry];
  instance->subsumptionEdges[node] = subsuming;
}

void SearchTree::addPathCondition(ITreeNode *iTreeNode,
                                  PathCondition *pathCondition,
                                  ref<Expr> condition) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  SearchTree::Node *node = instance->itreeNodeMap[iTreeNode];

  std::string constraintStr;
  llvm::raw_string_ostream stream(constraintStr);
  condition->print(stream);
  std::string s = stream.str();
  s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
  std::pair<std::string, bool> p(s, false);
  node->pathConditionTable[pathCondition] = p;
  instance->pathConditionMap[pathCondition] = node;
}

void SearchTree::addTableEntryMapping(ITreeNode *iTreeNode,
                                      SubsumptionTableEntry *entry) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  SearchTree::Node *node = instance->itreeNodeMap[iTreeNode];
  instance->tableEntryMap[entry] = node;
}

void SearchTree::includeInInterpolant(PathCondition *pathCondition) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  instance->pathConditionMap[pathCondition]->pathConditionTable[pathCondition].second = true;
}

/// @brief Save the graph
void SearchTree::save(std::string dotFileName) {
  assert(SearchTree::instance && "Search tree graph not initialized");

  std::string g(instance->render());
  std::ofstream out(dotFileName.c_str());
  if (!out.fail()) {
    out << g;
    out.close();
  }
}

/**/

PathConditionMarker::PathConditionMarker(PathCondition *pathCondition)
    : mayBeInInterpolant(false), pathCondition(pathCondition) {}

PathConditionMarker::~PathConditionMarker() {}

void PathConditionMarker::mayIncludeInInterpolant() {
  mayBeInInterpolant = true;
}

void PathConditionMarker::includeInInterpolant(AllocationGraph *g) {
  if (mayBeInInterpolant) {
    pathCondition->includeInInterpolant(g);
  }
}

/**/

PathCondition::PathCondition(ref<Expr> &constraint, Dependency *dependency,
                             llvm::Value *condition, PathCondition *prev)
    : constraint(constraint), shadowConstraint(constraint), shadowed(false),
      dependency(dependency),
      condition(dependency ? dependency->getLatestValue(condition, constraint)
                           : 0),
      inInterpolant(false), tail(prev) {}

PathCondition::~PathCondition() {}

ref<Expr> PathCondition::car() const { return constraint; }

PathCondition *PathCondition::cdr() const { return tail; }

void PathCondition::includeInInterpolant(AllocationGraph *g) {
  // We mark all values to which this constraint depends
  dependency->markAllValues(g, condition);

  // We mark this constraint itself as in the interpolant
  inInterpolant = true;

  // We mark constraint as in interpolant in the search tree graph as well.
  SearchTree::includeInInterpolant(this);
}

bool PathCondition::carInInterpolant() const { return inInterpolant; }

ref<Expr>
PathCondition::packInterpolant(std::vector<const Array *> &replacements) {
  ref<Expr> res;
  for (PathCondition *it = this; it != 0; it = it->tail) {
    if (it->inInterpolant) {
      if (!it->shadowed) {
        it->shadowConstraint =
            ShadowArray::getShadowExpression(it->constraint, replacements);
        it->shadowed = true;
      }
      if (res.get()) {
        res = AndExpr::alloc(res, it->shadowConstraint);
      } else {
        res = it->shadowConstraint;
      }
    }
  }
  return res;
}

void PathCondition::dump() {
  this->print(llvm::errs());
  llvm::errs() << "\n";
}

void PathCondition::print(llvm::raw_ostream &stream) {
  stream << "[";
  for (PathCondition *it = this; it != 0; it = it->tail) {
    it->constraint->print(stream);
    stream << ": " << (it->inInterpolant ? "interpolant constraint"
                                         : "non-interpolant constraint");
    if (it->tail != 0)
      stream << ",";
  }
  stream << "]";
}

/**/

SubsumptionTableEntry::SubsumptionTableEntry(ITreeNode *node)
    : nodeId(node->getNodeId()) {
  std::vector<const Array *> replacements;

  interpolant = node->getInterpolant(replacements);

  singletonStore = node->getLatestInterpolantCoreExpressions(replacements);
  for (std::map<llvm::Value *, ref<Expr> >::iterator
           it = singletonStore.begin(),
           itEnd = singletonStore.end();
       it != itEnd; ++it) {
    singletonStoreKeys.push_back(it->first);
  }

  compositeStore = node->getCompositeInterpolantCoreExpressions(replacements);
  for (std::map<llvm::Value *, std::vector<ref<Expr> > >::iterator
           it = compositeStore.begin(),
           itEnd = compositeStore.end();
       it != itEnd; ++it) {
    compositeStoreKeys.push_back(it->first);
  }

  existentials = replacements;
}

SubsumptionTableEntry::~SubsumptionTableEntry() {}

bool
SubsumptionTableEntry::hasExistentials(std::vector<const Array *> &existentials,
                                       ref<Expr> expr) {
  for (int i = 0, numKids = expr->getNumKids(); i < numKids; ++i) {
    if (llvm::isa<ReadExpr>(expr)) {
      ReadExpr *readExpr = llvm::dyn_cast<ReadExpr>(expr.get());
      const Array *array = (readExpr->updates).root;
      for (std::vector<const Array *>::iterator it = existentials.begin(),
                                                itEnd = existentials.end();
           it != itEnd; ++it) {
        if ((*it) == array)
          return true;
      }
    } else if (hasExistentials(existentials, expr->getKid(i)))
      return true;
  }
  return false;
}

ref<Expr>
SubsumptionTableEntry::simplifyWithFourierMotzkin(ref<Expr> existsExpr) {
  // This is a template for Fourier-Motzkin elimination. For now,
  // we simply return the input argument.
  return existsExpr;
}

ref<Expr> SubsumptionTableEntry::simplifyArithmeticBody(ref<Expr> existsExpr) {
  assert(llvm::isa<ExistsExpr>(existsExpr));

  std::vector<ref<Expr> > interpolantPack;
  std::vector<ref<Expr> > equalityPack;

  ExistsExpr *expr = static_cast<ExistsExpr *>(existsExpr.get());

  std::vector<const Array *> boundVariables = expr->variables;
  // We assume that the body is always a conjunction of interpolant in terms of
  // shadow (existentially-quantified) variables and state equality constraints,
  // which may contain both normal and shadow variables.
  ref<Expr> body = expr->body;

  // We only simplify a conjunction of interpolant and equalities
  if (!llvm::isa<AndExpr>(body))
    return existsExpr;

  // If the post-simplified body was a constant, simply return the body;
  if (llvm::isa<ConstantExpr>(body))
    return body;

  // The equality constraint is only a single disjunctive clause
  // of a CNF formula. In this case we simplify nothing.
  if (llvm::isa<OrExpr>(body->getKid(1)))
    return existsExpr;

  // Here we process equality constraints of shadow and normal variables.
  // The following procedure returns simplified version of the expression
  // by reducing any equality expression into constant (TRUE/FALSE).
  // if body is A and (Eq 2 4), body can be simplified into false.
  // if body is A and (Eq 2 2), body can be simplified into A.
  //
  // Along the way, It also collects the remaining equalities in equalityPack.
  // The equality constraints (body->getKid(1)) is a CNF of the form
  // c1 /\ ... /\ cn. This procedure collects into equalityPack all ci for
  // 1<=i<=n which are atomic equalities, to be used in simplifying the
  // interpolant.
  // ref<Expr> equalityConstraint =
  ref<Expr> fullEqualityConstraint =
      simplifyEqualityExpr(equalityPack, body->getKid(1));

  // Try to simplify the interpolant. If the resulting simplification
  // was a constant (true), then the equality constraints would contain
  // equality with constants only and no equality with shadow (existential)
  // variables, hence it should be safe to simply return the equality
  // constraint.
  interpolantPack.clear();
  ref<Expr> simplifiedInterpolant =
      simplifyInterpolantExpr(interpolantPack, body->getKid(0));
  if (llvm::isa<ConstantExpr>(simplifiedInterpolant))
    return fullEqualityConstraint;

  ref<Expr> newInterpolant;

  for (std::vector<ref<Expr> >::iterator it = interpolantPack.begin(),
                                         itEnd = interpolantPack.end();
       it != itEnd; ++it) {

    ref<Expr> interpolantAtom = (*it); // For example C cmp D

    for (std::vector<ref<Expr> >::iterator itEq = equalityPack.begin(),
                                           itEqEnd = equalityPack.end();
         itEq != itEqEnd; ++itEq) {

      ref<Expr> equalityConstraint =
          *itEq; // For example, say this constraint is A == B
      if (equalityConstraint->isFalse()) {
        return ConstantExpr::alloc(0, Expr::Bool);
      } else if (equalityConstraint->isTrue()) {
        return ConstantExpr::alloc(1, Expr::Bool);
      }
      // Left-hand side of the equality formula (A in our example) that contains
      // the shadow expression (we assume that the existentially-quantified
      // shadow variable is always on the left side).
      ref<Expr> equalityConstraintLeft = equalityConstraint->getKid(0);

      // Right-hand side of the equality formula (B in our example) that does
      // not contain existentially-quantified shadow variables.
      ref<Expr> equalityConstraintRight = equalityConstraint->getKid(1);

      ref<Expr> newIntpLeft;
      ref<Expr> newIntpRight;

      // When the if condition holds, we perform substitution
      if (containShadowExpr(equalityConstraintLeft,
                            interpolantAtom->getKid(0))) {
        // Here we perform substitution, where given
        // an interpolant atom and an equality constraint,
        // we try to find a subexpression in the equality constraint
        // that matches the lhs expression of the interpolant atom.

        // Here we assume that the equality constraint is A == B and the
        // interpolant atom is C cmp D.

        // newIntpLeft == B
        newIntpLeft = equalityConstraintRight;

        // If equalityConstraintLeft does not have any arithmetic operation
        // we could directly assign newIntpRight = D, otherwise,
        // newIntpRight == A[D/C]
        if (!llvm::isa<BinaryExpr>(equalityConstraintLeft))
          newIntpRight = interpolantAtom->getKid(1);
        else {
          // newIntpRight is A, but with every occurrence of C replaced with D
          // i.e., newIntpRight == A[D/C]
          newIntpRight =
              replaceExpr(equalityConstraintLeft, interpolantAtom->getKid(0),
                          interpolantAtom->getKid(1));
        }

        interpolantAtom =
            createBinaryOfSameKind(interpolantAtom, newIntpLeft, newIntpRight);
      }
    }

    // We add the modified interpolant conjunct into a conjunction of
    // new interpolants.
    if (newInterpolant.get()) {
      newInterpolant = AndExpr::alloc(newInterpolant, interpolantAtom);
    } else {
      newInterpolant = interpolantAtom;
    }
  }

  ref<Expr> newBody;

  if (newInterpolant.get()) {
    if (!hasExistentials(expr->variables, newInterpolant))
      return newInterpolant;

    newBody = AndExpr::alloc(newInterpolant, fullEqualityConstraint);
  } else {
    newBody = AndExpr::alloc(simplifiedInterpolant, fullEqualityConstraint);
  }

  return simplifyWithFourierMotzkin(existsExpr->rebuild(&newBody));
}

ref<Expr> SubsumptionTableEntry::replaceExpr(ref<Expr> originalExpr,
                                             ref<Expr> replacedExpr,
                                             ref<Expr> substituteExpr) {
  // We only handle binary expressions
  if (!llvm::isa<BinaryExpr>(originalExpr) ||
      llvm::isa<ConcatExpr>(originalExpr))
    return originalExpr;

  if (originalExpr->getKid(0) == replacedExpr)
    return createBinaryOfSameKind(originalExpr, substituteExpr,
                                  originalExpr->getKid(1));

  if (originalExpr->getKid(1) == replacedExpr)
    return createBinaryOfSameKind(originalExpr, originalExpr->getKid(0),
                                  substituteExpr);

  return createBinaryOfSameKind(
      originalExpr,
      replaceExpr(originalExpr->getKid(0), replacedExpr, substituteExpr),
      replaceExpr(originalExpr->getKid(1), replacedExpr, substituteExpr));
}

bool SubsumptionTableEntry::containShadowExpr(ref<Expr> expr,
                                              ref<Expr> shadowExpr) {
  if (expr.operator==(shadowExpr))
    return true;
  if (expr->getNumKids() < 2 && expr.operator!=(shadowExpr))
    return false;

  return containShadowExpr(expr->getKid(0), shadowExpr) ||
         containShadowExpr(expr->getKid(1), shadowExpr);
}

ref<Expr> SubsumptionTableEntry::createBinaryOfSameKind(ref<Expr> originalExpr,
                                                        ref<Expr> newLhs,
                                                        ref<Expr> newRhs) {
  std::vector<Expr::CreateArg> exprs;
  Expr::CreateArg arg1(newLhs);
  Expr::CreateArg arg2(newRhs);
  exprs.push_back(arg1);
  exprs.push_back(arg2);
  return Expr::createFromKind(originalExpr->getKind(), exprs);
}

ref<Expr> SubsumptionTableEntry::simplifyInterpolantExpr(
    std::vector<ref<Expr> > &interpolantPack, ref<Expr> expr) {
  if (expr->getNumKids() < 2)
    return expr;

  if (llvm::isa<EqExpr>(expr) && llvm::isa<ConstantExpr>(expr->getKid(0)) &&
      llvm::isa<ConstantExpr>(expr->getKid(1))) {
    return (expr->getKid(0).operator==(expr->getKid(1)))
               ? ConstantExpr::alloc(1, Expr::Bool)
               : ConstantExpr::alloc(0, Expr::Bool);
  } else if (llvm::isa<NeExpr>(expr) &&
             llvm::isa<ConstantExpr>(expr->getKid(0)) &&
             llvm::isa<ConstantExpr>(expr->getKid(1))) {
    return (expr->getKid(0).operator!=(expr->getKid(1)))
               ? ConstantExpr::alloc(1, Expr::Bool)
               : ConstantExpr::alloc(0, Expr::Bool);
  }

  ref<Expr> lhs = expr->getKid(0);
  ref<Expr> rhs = expr->getKid(1);

  if (!llvm::isa<AndExpr>(expr)) {

    // If the current expression has a form like (Eq false P), where P is some
    // comparison, we change it into the negation of P.
    if (llvm::isa<EqExpr>(expr) && expr->getKid(0)->getWidth() == Expr::Bool &&
        expr->getKid(0)->isFalse()) {
      if (llvm::isa<SltExpr>(rhs)) {
        expr = SgeExpr::create(rhs->getKid(0), rhs->getKid(1));
      } else if (llvm::isa<SgeExpr>(rhs)) {
        expr = SltExpr::create(rhs->getKid(0), rhs->getKid(1));
      } else if (llvm::isa<SleExpr>(rhs)) {
        expr = SgtExpr::create(rhs->getKid(0), rhs->getKid(1));
      } else if (llvm::isa<SgtExpr>(rhs)) {
        expr = SleExpr::create(rhs->getKid(0), rhs->getKid(1));
      }
    }

    // Collect unique interpolant expressions in one vector
    if (std::find(interpolantPack.begin(), interpolantPack.end(), expr) ==
        interpolantPack.end())
      interpolantPack.push_back(expr);

    return expr;
  }

  return AndExpr::alloc(simplifyInterpolantExpr(interpolantPack, lhs),
                        simplifyInterpolantExpr(interpolantPack, rhs));
}

ref<Expr> SubsumptionTableEntry::simplifyEqualityExpr(
    std::vector<ref<Expr> > &equalityPack, ref<Expr> expr) {
  if (expr->getNumKids() < 2)
    return expr;

  if (llvm::isa<EqExpr>(expr)) {
    if (llvm::isa<ConstantExpr>(expr->getKid(0)) &&
        llvm::isa<ConstantExpr>(expr->getKid(1))) {
      return (expr->getKid(0).operator==(expr->getKid(1)))
                 ? ConstantExpr::alloc(1, Expr::Bool)
                 : ConstantExpr::alloc(0, Expr::Bool);
    }

    // Collect unique equality and in-equality expressions in one vector
    if (std::find(equalityPack.begin(), equalityPack.end(), expr) ==
        equalityPack.end())
      equalityPack.push_back(expr);

    return expr;
  }

  if (llvm::isa<AndExpr>(expr)) {
    ref<Expr> lhs = simplifyEqualityExpr(equalityPack, expr->getKid(0));
    if (lhs->isFalse())
      return lhs;

    ref<Expr> rhs = simplifyEqualityExpr(equalityPack, expr->getKid(1));
    if (rhs->isFalse())
      return rhs;

    if (lhs->isTrue())
      return rhs;

    if (rhs->isTrue())
      return lhs;

    return AndExpr::alloc(lhs, rhs);
  } else if (llvm::isa<OrExpr>(expr)) {
    // We provide throw-away dummy equalityPack, as we do not use the atomic
    // equalities within disjunctive clause to simplify the interpolant.
    std::vector<ref<Expr> > dummy;
    ref<Expr> lhs = simplifyEqualityExpr(dummy, expr->getKid(0));
    if (lhs->isTrue())
      return lhs;

    ref<Expr> rhs = simplifyEqualityExpr(dummy, expr->getKid(1));
    if (rhs->isTrue())
      return rhs;

    if (lhs->isFalse())
      return rhs;

    if (rhs->isFalse())
      return lhs;

    return OrExpr::alloc(lhs, rhs);
  }

  assert(!"Invalid expression type.");
}

ref<Expr> SubsumptionTableEntry::simplifyExistsExpr(ref<Expr> existsExpr) {
  assert(llvm::isa<ExistsExpr>(existsExpr));

  ref<Expr> ret = simplifyArithmeticBody(existsExpr);
  if (llvm::isa<ExistsExpr>(ret))
    return ret;

  return ret;
}

bool SubsumptionTableEntry::subsumed(TimingSolver *solver,
                                     ExecutionState &state, double timeout) {
  // Check if we are at the right program point
  if (state.itreeNode == 0 || reinterpret_cast<uintptr_t>(state.pc->inst) !=
                                  state.itreeNode->getNodeId() ||
      state.itreeNode->getNodeId() != nodeId)
    return false;

  // Quick check for subsumption in case the interpolant is empty
  if (empty())
    return true;

  std::map<llvm::Value *, ref<Expr> > stateSingletonStore =
      state.itreeNode->getLatestCoreExpressions();
  std::map<llvm::Value *, std::vector<ref<Expr> > > stateCompositeStore =
      state.itreeNode->getCompositeCoreExpressions();

  ref<Expr> stateEqualityConstraints;
  for (std::vector<llvm::Value *>::iterator it = singletonStoreKeys.begin(),
                                            itEnd = singletonStoreKeys.end();
       it != itEnd; ++it) {
      const ref<Expr> lhs = singletonStore[*it];
      const ref<Expr> rhs = stateSingletonStore[*it];

      // If the current state does not constrain the same
      // allocation, subsumption fails.
      if (!rhs.get())
        return false;

      stateEqualityConstraints =
          (!stateEqualityConstraints.get()
               ? EqExpr::alloc(lhs, rhs)
               : AndExpr::alloc(EqExpr::alloc(lhs, rhs),
                                stateEqualityConstraints));
  }

  for (std::vector<llvm::Value *>::iterator it = compositeStoreKeys.begin(),
                                            itEnd = compositeStoreKeys.end();
       it != itEnd; ++it) {
    std::vector<ref<Expr> > lhsList = compositeStore[*it];
    std::vector<ref<Expr> > rhsList = stateCompositeStore[*it];

    // If the current state does not constrain the same
    // allocation, subsumption fails.
    if (rhsList.empty())
      return false;

    ref<Expr> auxDisjuncts;
    bool auxDisjunctsEmpty = true;

    for (std::vector<ref<Expr> >::iterator lhsIter = lhsList.begin(),
                                           lhsIterEnd = lhsList.end();
         lhsIter != lhsIterEnd; ++lhsIter) {
      for (std::vector<ref<Expr> >::iterator rhsIter = rhsList.begin(),
                                             rhsIterEnd = rhsList.end();
           rhsIter != rhsIterEnd; ++rhsIter) {
        const ref<Expr> lhs = *lhsIter;
        const ref<Expr> rhs = *rhsIter;

        if (auxDisjunctsEmpty) {
          auxDisjuncts = EqExpr::alloc(lhs, rhs);
          auxDisjunctsEmpty = false;
        } else {
          auxDisjuncts = OrExpr::alloc(EqExpr::alloc(lhs, rhs), auxDisjuncts);
        }
      }
    }

    if (!auxDisjunctsEmpty) {
      stateEqualityConstraints =
          stateEqualityConstraints.get()
              ? AndExpr::alloc(auxDisjuncts, stateEqualityConstraints)
              : auxDisjuncts;
    }
  }

  // We create path condition needed constraints marking structure
  std::map<ref<Expr>, PathConditionMarker *> markerMap =
      state.itreeNode->makeMarkerMap();

  Solver::Validity result;
  ref<Expr> query;

  // Here we build the query, after which it is always a conjunction of
  // the interpolant and the state equality constraints.
  if (interpolant.get()) {
    query =
        stateEqualityConstraints.get()
            ? AndExpr::alloc(interpolant, stateEqualityConstraints)
            : AndExpr::alloc(interpolant, ConstantExpr::create(1, Expr::Bool));
  } else if (stateEqualityConstraints.get()) {
    query = AndExpr::alloc(ConstantExpr::create(1, Expr::Bool),
                           stateEqualityConstraints);
  } else {
    // Here both the interpolant constraints and state equality
    // constraints are empty, therefore everything gets subsumed
    return true;
  }

  if (!existentials.empty()) {
    ref<Expr> existsExpr = ExistsExpr::create(existentials, query);
    // llvm::errs() << "Before simplification:\n";
    // ExprPPrinter::printQuery(llvm::errs(), state.constraints, existsExpr);
    query = simplifyExistsExpr(existsExpr);
  }

  bool success = false;

  Z3Solver *z3solver = 0;

  // We call the solver only when the simplified query is
  // not a constant.
  if (!llvm::isa<ConstantExpr>(query)) {
    // llvm::errs() << "Querying for subsumption check:\n";
    // ExprPPrinter::printQuery(llvm::errs(), state.constraints, query);

    if (!existentials.empty() && llvm::isa<ExistsExpr>(query)) {
      // llvm::errs() << "Existentials not empty\n";

      // Instantiate a new Z3 solver to make sure we use Z3
      // without pre-solving optimizations. It would be nice
      // in the future to just run solver->evaluate so that
      // the optimizations can be used, but this requires
      // handling of quantified expressions by KLEE's pre-solving
      // procedure, which does not exist currently.
      z3solver = new Z3Solver();

      z3solver->setCoreSolverTimeout(timeout);

      success = z3solver->directComputeValidity(Query(state.constraints, query),
                                                result);
      z3solver->setCoreSolverTimeout(0);
    } else {
      // llvm::errs() << "No existential\n";

      // We call the solver in the standard way if the
      // formula is unquantified.
      solver->setTimeout(timeout);
      success = solver->evaluate(state, query, result);
      solver->setTimeout(0);
    }
  } else {
    if (query->isTrue())
      return true;
    return false;
  }

  if (success && result == Solver::True) {
    // llvm::errs() << "Solver decided validity\n";
    std::vector<ref<Expr> > unsatCore;
    if (z3solver) {
      unsatCore = z3solver->getUnsatCore();
      delete z3solver;
    } else {
      unsatCore = solver->getUnsatCore();
    }

    for (std::vector<ref<Expr> >::iterator it1 = unsatCore.begin();
         it1 != unsatCore.end(); it1++) {
      markerMap[*it1]->mayIncludeInInterpolant();
    }

  } else {
    // Here the solver could not decide that the subsumption is valid.
    // It may have decided invalidity, however,
    // CexCachingSolver::computeValidity,
    // which was eventually called from solver->evaluate
    // is conservative, where it returns Solver::Unknown even in case when
    // invalidity is established by the solver.
    // llvm::errs() << "Solver could not decide validity\n";

    if (z3solver)
      delete z3solver;

    return false;
  }

  // State subsumed, we mark needed constraints on the
  // path condition.
  AllocationGraph *g = new AllocationGraph();
  for (std::map<ref<Expr>, PathConditionMarker *>::iterator
           it = markerMap.begin(),
           itEnd = markerMap.end();
       it != itEnd; it++) {
    it->second->includeInInterpolant(g);
  }
  ITreeNode::deleteMarkerMap(markerMap);

  // llvm::errs() << "AllocationGraph\n";
  // g->dump();

  // We mark memory allocations needed for the unsatisfiabilty core
  state.itreeNode->computeInterpolantAllocations(g);

  delete g; // Delete the AllocationGraph object
  return true;
}

void SubsumptionTableEntry::dump() const {
  this->print(llvm::errs());
  llvm::errs() << "\n";
}

void SubsumptionTableEntry::print(llvm::raw_ostream &stream) const {
  stream << "------------ Subsumption Table Entry ------------\n";
  stream << "Program point = " << nodeId << "\n";
  stream << "interpolant = ";
  if (interpolant.get())
    interpolant->print(stream);
  else
    stream << "(empty)";
  stream << "\n";

  if (!singletonStore.empty()) {
    stream << "singleton allocations = [";
    for (std::map<llvm::Value *, ref<Expr> >::const_iterator
             itBegin = singletonStore.begin(),
             itEnd = singletonStore.end(), it = itBegin;
         it != itEnd; ++it) {
      if (it != itBegin)
        stream << ",";
      stream << "(";
      it->first->print(stream);
      stream << ",";
      it->second->print(stream);
      stream << ")";
    }
    stream << "]\n";
  }

  if (!compositeStore.empty()) {
    stream << "composite allocations = [";
    for (std::map<llvm::Value *, std::vector<ref<Expr> > >::const_iterator
             it0Begin = compositeStore.begin(),
             it0End = compositeStore.end(), it0 = it0Begin;
         it0 != it0End; ++it0) {
      if (it0 != it0Begin)
        stream << ",";
      stream << "(";
      it0->first->print(stream);
      stream << ",[";
      for (std::vector<ref<Expr> >::const_iterator
               it1Begin = it0->second.begin(),
               it1End = it0->second.end(), it1 = it1Begin;
           it1 != it1End; ++it1) {
        if (it1 != it1Begin)
          stream << ",";
        (*it1)->print(stream);
      }
      stream << "])";
    }
    stream << "]\n";
  }

  if (!existentials.empty()) {
    stream << "existentials = [";
    for (std::vector<const Array *>::const_iterator
             itBegin = existentials.begin(),
             itEnd = existentials.end(), it = itBegin;
         it != itEnd; ++it) {
      if (it != itBegin)
        stream << ", ";
      stream << (*it)->name;
    }
    stream << "]\n";
  }
}

/**/

ITree::ITree(ExecutionState *_root) {
  currentINode = 0;
  if (!_root->itreeNode) {
    currentINode = new ITreeNode(0);
  }
  root = currentINode;
}

ITree::~ITree() {
  for (std::vector<SubsumptionTableEntry *>::iterator
           it = subsumptionTable.begin(),
           itEnd = subsumptionTable.end();
       it != itEnd; ++it) {
    delete (*it);
  }
  subsumptionTable.clear();
}

bool ITree::checkCurrentStateSubsumption(TimingSolver *solver,
                                         ExecutionState &state,
                                         double timeout) {
  assert(state.itreeNode == currentINode);

  for (std::vector<SubsumptionTableEntry *>::iterator it =
           subsumptionTable.begin();
       it != subsumptionTable.end(); it++) {

    if ((*it)->subsumed(solver, state, timeout)) {

      // We mark as subsumed such that the node will not be
      // stored into table (the table already contains a more
      // general entry).
      currentINode->isSubsumed = true;

      // Mark the node as subsumed, and create a subsumption edge
      SearchTree::markAsSubsumed(currentINode, (*it));
      return true;
    }
  }
  return false;
}

std::vector<SubsumptionTableEntry *> ITree::getStore() {
  return subsumptionTable;
}

void ITree::store(SubsumptionTableEntry *subItem) {
  subsumptionTable.push_back(subItem);
}

void ITree::setCurrentINode(ITreeNode *node, uintptr_t programPoint) {
  currentINode = node;
  currentINode->setNodeLocation(programPoint);
  SearchTree::setCurrentNode(node, programPoint);
}

void ITree::remove(ITreeNode *node) {
  assert(!node->left && !node->right);
  do {
    ITreeNode *p = node->parent;

    // As the node is about to be deleted, it must have been completely
    // traversed, hence the correct time to table the interpolant.
    if (!node->isSubsumed) {
      SubsumptionTableEntry *entry = new SubsumptionTableEntry(node);
      store(entry);
      SearchTree::addTableEntryMapping(node, entry);
    }

    delete node;
    if (p) {
      if (node == p->left) {
        p->left = 0;
      } else {
        assert(node == p->right);
        p->right = 0;
      }
    }
    node = p;
  } while (node && !node->left && !node->right);
}

std::pair<ITreeNode *, ITreeNode *>
ITree::split(ITreeNode *parent, ExecutionState *left, ExecutionState *right) {
  parent->split(left, right);
  SearchTree::addChildren(parent, parent->left, parent->right);
  return std::pair<ITreeNode *, ITreeNode *>(parent->left, parent->right);
}

void ITree::markPathCondition(ExecutionState &state, TimingSolver *solver) {
  std::vector<ref<Expr> > unsatCore = solver->getUnsatCore();

  AllocationGraph *g = new AllocationGraph();

  llvm::BranchInst *binst =
      llvm::dyn_cast<llvm::BranchInst>(state.prevPC->inst);
  if (binst) {
    currentINode->dependency->markAllValues(g, binst->getCondition());
  }

  PathCondition *pc = currentINode->pathCondition;

  if (pc != 0) {
    for (std::vector<ref<Expr> >::reverse_iterator it = unsatCore.rbegin();
         it != unsatCore.rend(); it++) {
      while (pc != 0) {
        if (pc->car().compare(it->get()) == 0) {
          pc->includeInInterpolant(g);
          pc = pc->cdr();
          break;
        }
        pc = pc->cdr();
      }
      if (pc == 0)
        break;
    }
  }

  // llvm::errs() << "AllocationGraph\n";
  // g->dump();

  // Compute memory allocations needed by the unsatisfiability core
  currentINode->computeInterpolantAllocations(g);

  delete g; // Delete the AllocationGraph object
}

void ITree::executeAbstractBinaryDependency(llvm::Instruction *instr,
                                            ref<Expr> valueExpr,
                                            ref<Expr> tExpr, ref<Expr> fExpr) {
  currentINode->executeBinaryDependency(instr, valueExpr, tExpr, fExpr);
}

void ITree::executeAbstractMemoryDependency(llvm::Instruction *instr,
                                            ref<Expr> value,
                                            ref<Expr> address) {
  currentINode->executeAbstractMemoryDependency(instr, value, address);
}

void ITree::executeAbstractDependency(llvm::Instruction *instr,
                                      ref<Expr> value) {
  currentINode->executeAbstractDependency(instr, value);
}

void ITree::printNode(llvm::raw_ostream &stream, ITreeNode *n,
                      std::string edges) {
  if (n->left != 0) {
    stream << "\n";
    stream << edges << "+-- L:" << n->left->nodeId;
    if (this->currentINode == n->left) {
      stream << " (active)";
    }
    if (n->right != 0) {
      printNode(stream, n->left, edges + "|   ");
    } else {
      printNode(stream, n->left, edges + "    ");
    }
  }
  if (n->right != 0) {
    stream << "\n";
    stream << edges << "+-- R:" << n->right->nodeId;
    if (this->currentINode == n->right) {
      stream << " (active)";
    }
    printNode(stream, n->right, edges + "    ");
  }
}

void ITree::print(llvm::raw_ostream &stream) {
  stream << "------------------------- ITree Structure "
            "---------------------------\n";
  stream << this->root->nodeId;
  if (this->root == this->currentINode) {
    stream << " (active)";
  }
  this->printNode(stream, this->root, "");
  stream << "\n------------------------- Subsumption Table "
            "-------------------------\n";
  for (std::vector<SubsumptionTableEntry *>::iterator
           it = subsumptionTable.begin(),
           itEnd = subsumptionTable.end();
       it != itEnd; ++it) {
    (*it)->print(stream);
  }
}

void ITree::dump() { this->print(llvm::errs()); }

/**/

ITreeNode::ITreeNode(ITreeNode *_parent)
    : parent(_parent), left(0), right(0), nodeId(0), isSubsumed(false),
      graph(_parent ? _parent->graph : 0) {

  pathCondition = (_parent != 0) ? _parent->pathCondition : 0;

  // Inherit the abstract dependency or NULL
  dependency = new Dependency(_parent ? _parent->dependency : 0);
}

ITreeNode::~ITreeNode() {
  // Only delete the path condition if it's not
  // also the parent's path condition
  PathCondition *itEnd = parent ? parent->pathCondition : 0;

  PathCondition *it = pathCondition;
  while (it != itEnd) {
    PathCondition *tmp = it;
    it = it->cdr();
    delete tmp;
  }

  if (dependency)
    delete dependency;
}

uintptr_t ITreeNode::getNodeId() { return nodeId; }

ref<Expr>
ITreeNode::getInterpolant(std::vector<const Array *> &replacements) const {
  return this->pathCondition->packInterpolant(replacements);
}

void ITreeNode::addConstraint(ref<Expr> &constraint, llvm::Value *condition) {
  pathCondition =
      new PathCondition(constraint, dependency, condition, pathCondition);
  graph->addPathCondition(this, pathCondition, constraint);
}

void ITreeNode::split(ExecutionState *leftData, ExecutionState *rightData) {
  assert(left == 0 && right == 0);
  leftData->itreeNode = left = new ITreeNode(this);
  rightData->itreeNode = right = new ITreeNode(this);
}

std::map<ref<Expr>, PathConditionMarker *> ITreeNode::makeMarkerMap() const {
  std::map<ref<Expr>, PathConditionMarker *> result;
  for (PathCondition *it = pathCondition; it != 0; it = it->cdr()) {
    PathConditionMarker *marker = new PathConditionMarker(it);
    if (llvm::isa<OrExpr>(it->car().get())) {
      // Break up disjunction into its components, because each disjunct
      // is solved separately
      result.insert(std::pair<ref<Expr>, PathConditionMarker *>(
          it->car()->getKid(0), marker));
      result.insert(std::pair<ref<Expr>, PathConditionMarker *>(
          it->car()->getKid(1), marker));
    }
    result.insert(
        std::pair<ref<Expr>, PathConditionMarker *>(it->car(), marker));
  }
  return result;
}

void ITreeNode::deleteMarkerMap(
    std::map<ref<Expr>, PathConditionMarker *> &markerMap) {
  for (std::map<ref<Expr>, PathConditionMarker *>::iterator
           it = markerMap.begin(),
           itEnd = markerMap.end();
       it != itEnd; ++it) {
    delete it->second;
  }
  markerMap.clear();
}

void ITreeNode::executeBinaryDependency(llvm::Instruction *i,
                                        ref<Expr> valueExpr, ref<Expr> tExpr,
                                        ref<Expr> fExpr) {
  dependency->executeBinary(i, valueExpr, tExpr, fExpr);
}

void ITreeNode::executeAbstractMemoryDependency(llvm::Instruction *instr,
                                                ref<Expr> value,
                                                ref<Expr> address) {
  dependency->executeMemoryOperation(instr, value, address);
}

void ITreeNode::executeAbstractDependency(llvm::Instruction *instr,
                                          ref<Expr> value) {
  dependency->execute(instr, value);
}

void ITreeNode::bindCallArguments(llvm::Instruction *site,
                                  std::vector<ref<Expr> > &arguments) {
  dependency->bindCallArguments(site, arguments);
}

void ITreeNode::popAbstractDependencyFrame(llvm::CallInst *site,
                                           llvm::Instruction *inst,
                                           ref<Expr> returnValue) {
  // TODO: This is probably where we should simplify
  // the dependency graph by removing callee values.

  dependency->bindReturnValue(site, inst, returnValue);
}

std::map<llvm::Value *, ref<Expr> >
ITreeNode::getLatestCoreExpressions() const {
  std::map<llvm::Value *, ref<Expr> > ret;
  std::vector<const Array *> dummyReplacements;

  // Since a program point index is a first statement in a basic block,
  // the allocations to be stored in subsumption table should be obtained
  // from the parent node.
  if (parent)
    ret =
        parent->dependency->getLatestCoreExpressions(dummyReplacements, false);
  return ret;
}

std::map<llvm::Value *, std::vector<ref<Expr> > >
ITreeNode::getCompositeCoreExpressions() const {
  std::map<llvm::Value *, std::vector<ref<Expr> > > ret;
  std::vector<const Array *> dummyReplacements;

  // Since a program point index is a first statement in a basic block,
  // the allocations to be stored in subsumption table should be obtained
  // from the parent node.
  if (parent)
    ret = parent->dependency->getCompositeCoreExpressions(dummyReplacements,
                                                          false);
  return ret;
}

std::map<llvm::Value *, ref<Expr> >
ITreeNode::getLatestInterpolantCoreExpressions(
    std::vector<const Array *> &replacements) const {
  std::map<llvm::Value *, ref<Expr> > ret;

  // Since a program point index is a first statement in a basic block,
  // the allocations to be stored in subsumption table should be obtained
  // from the parent node.
  if (parent)
    ret = parent->dependency->getLatestCoreExpressions(replacements, true);
  return ret;
}

std::map<llvm::Value *, std::vector<ref<Expr> > >
ITreeNode::getCompositeInterpolantCoreExpressions(
    std::vector<const Array *> &replacements) const {
  std::map<llvm::Value *, std::vector<ref<Expr> > > ret;

  // Since a program point index is a first statement in a basic block,
  // the allocations to be stored in subsumption table should be obtained
  // from the parent node.
  if (parent)
    ret = parent->dependency->getCompositeCoreExpressions(replacements, true);
  return ret;
}

void ITreeNode::computeInterpolantAllocations(AllocationGraph *g) {
  dependency->computeInterpolantAllocations(g);
}

void ITreeNode::dump() const {
  llvm::errs() << "------------------------- ITree Node "
                  "--------------------------------\n";
  this->print(llvm::errs());
  llvm::errs() << "\n";
}

void ITreeNode::print(llvm::raw_ostream &stream) const {
  this->print(stream, 0);
}

void ITreeNode::print(llvm::raw_ostream &stream, const unsigned tabNum) const {
  std::string tabs = makeTabs(tabNum);
  std::string tabs_next = appendTab(tabs);

  stream << tabs << "ITreeNode\n";
  stream << tabs_next << "node Id = " << nodeId << "\n";
  stream << tabs_next << "pathCondition = ";
  if (pathCondition == 0) {
    stream << "NULL";
  } else {
    pathCondition->print(stream);
  }
  stream << "\n";
  stream << tabs_next << "Left:\n";
  if (!left) {
    stream << tabs_next << "NULL\n";
  } else {
    left->print(stream, tabNum + 1);
    stream << "\n";
  }
  stream << tabs_next << "Right:\n";
  if (!right) {
    stream << tabs_next << "NULL\n";
  } else {
    right->print(stream, tabNum + 1);
    stream << "\n";
  }
  if (dependency) {
    stream << tabs_next << "------- Abstract Dependencies ----------\n";
    dependency->print(stream, tabNum + 1);
  }
}
